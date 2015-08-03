/*
	Copyright (C) 2013-2015 Róbert "gman" Vašek <gman@codefreax.org>

	This file is part of hawk-test.

	hawk-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	hawk-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with hawk-test.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iomanip>
#include <cstring>
#include "io.h"
#include "utils.h"

using namespace hawk;
using namespace std;

namespace {

void print_status(unsigned id, const IO_task::Item& i, IO_task::Status st)
{
	cout << "[[ IO" << id << ": ";

	using St = IO_task::Status;
	switch (st)
	{
	case St::failed:
		cout << "failed"; break;
	case St::finished:
		cout << "finished"; break;
	case St::paused:
		cout << "paused"; break;
	case St::pending:
		cout << "pending"; break;
	case St::preparing:
		cout << "preparing"; break;
	}

	cout << " ]]" << endl;
}

void print_error(const Filesystem_error& e)
{
	cout << " ## Filesystem_error: " << e.what() << endl;
}

void print_error(const IO_task_error& e)
{
	cout << " ## IO_task_error: " << e.get_source()
		 << ' ' << e.get_destination() << '\n';
	cout << "  # " << strerror(e.get_errno()) << endl;
}

void print_start_task(const char* task, const Path& src, const Path& dst)
{
	cout << ' ' << task << ' ' << src;
	if (!dst.empty()) cout << " to " << dst;
	cout << endl;
}

void print_start_task(
		const char* task, const vector<Path>& srcs, const Path& dst)
{
	if (srcs.size() == 1)
	{
		print_start_task(task, srcs[0], dst);
		return;
	}

	cout << ' ' << task << ":\n";

	int sz = srcs.size();
	for (const Path& p : srcs)
	{
		cout << "  " << p;
		if (--sz > 0) cout << ',';
		cout << '\n';
	}

	if (!dst.empty()) cout << " to " << dst;
	cout << endl;
}

void task_monitor(IO_task* parent, const Task_progress& tp)
{
	IO_base* b = dynamic_cast<IO_base*>(parent);

	cout << "[ IO" << b->id << ": " << tp.src;
	if (!tp.dst.empty()) cout << " -> " << tp.dst;
	cout <<  " ]" << endl;
}

void print_time(chrono::seconds s)
{
	using namespace chrono;

	hours h = duration_cast<hours>(s);
	s -= h;
	minutes m = duration_cast<minutes>(s);
	s -= m;

	cout << setw(2) << setfill('0') << h.count() << ':';
	cout << setw(2) << setfill('0') << m.count() << ':';
	cout << setw(2) << setfill('0') << s.count();
}

void file_monitor(IO_task* parent, const File_progress& fp)
{
	using namespace chrono;
	using namespace chrono_literals;

	IO_base* b = reinterpret_cast<IO_base*>(parent);
	cout << "[ IO" << b->id << ": ETA ";
	print_time(fp.eta_end);

	uintmax_t ttotal = parent->total_size();
	if (ttotal > fp.total)
	{
		seconds total_eta = fp.eta_end*(ttotal-parent->get_offset()-fp.offset)
							/ (fp.total - fp.offset);

		cout << '/';
		print_time(total_eta);
	}

	cout << " @ ";

	const char* units[] = {"B", "KB", "MB", "GB"};
	double sz = fp.rate;
	int i = 0;

	for (; i < 3; i++)
	{
		if (sz < 1024) break;
		sz /= 1024;
	}

	printf("%.*f%s/s ", i, sz, units[i]);
	printf("%.2f%%", 100 * (double)fp.offset / (double)fp.total);
	cout << " ]" << endl;
}

} // unnamed-namespace

// IO_copy impl.

void IO_copy::on_error(const Filesystem_error& e) const noexcept
{ print_error(e); }

IO_copy::IO_copy(const Path& src, const Path& dst, bool deref, bool update)
	: IO_task_copy{src, dst, deref, update}
{
	print_start_task("Copying", src, dst);
}

IO_copy::IO_copy(const vector<Path>& srcs, const Path& dst,
				 bool deref, bool update)
	: IO_task_copy{srcs, dst, deref, update}
{
	print_start_task("Copying", srcs, dst);
}

void IO_copy::on_error(const IO_task_error& e) const noexcept
{ print_error(e); }

void IO_copy::on_status_change(IO_task::Status st) const noexcept
{ print_status(id, current_item(), st); }

// IO_move impl.

void IO_move::on_error(const Filesystem_error& e) const noexcept
{ print_error(e); }

IO_move::IO_move(const Path& src, const Path& dst, bool update)
	: IO_task_move{src, dst, update}
{
	print_start_task("Moving", src, dst);
}

IO_move::IO_move(const vector<Path>& srcs, const Path& dst, bool update)
	: IO_task_move{srcs, dst, update}
{
	print_start_task("Moving", srcs, dst);
}

void IO_move::on_error(const IO_task_error& e) const noexcept
{ print_error(e); }

void IO_move::on_status_change(IO_task::Status st) const noexcept
{ print_status(id, current_item(), st); }

// IO_remove impl.

void IO_remove::on_error(const Filesystem_error& e) const noexcept
{ print_error(e); }

IO_remove::IO_remove(const Path& p) : IO_task_remove{p}
{
	print_start_task("Removing", p, "");
}

IO_remove::IO_remove(const std::vector<Path>& pvec) : IO_task_remove{pvec}
{
	print_start_task("Removing", pvec, "");
}

void IO_remove::on_error(const IO_task_error& e) const noexcept
{ print_error(e); }

void IO_remove::on_status_change(IO_task::Status st) const noexcept
{ print_status(id, current_item(), st); }

// IO_manager impl.

IO_manager::IO_manager()
{
	set_io_task_callbacks(&task_monitor, &file_monitor);
}

IO_task& IO_manager::get(unsigned id)
{
	return *(m_tasks[(id == 0) ? m_tasks.last_id() : id]);
}

bool IO_manager::remove(unsigned id)
{
	return m_tasks.erase(id);
}

void IO_manager::remove_finished()
{
	auto& tasks = m_tasks.get_container();
	for (auto it = tasks.begin(); it != tasks.end();)
	{
		using St = hawk::IO_task::Status;
		St st = it->second->get_status();

		if (st == St::finished || st == St::failed)
			it = tasks.erase(it);
		else
			++it;
	}
}
