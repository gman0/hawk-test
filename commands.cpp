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

#include <iostream>
#include <algorithm>
#include <cstring>
#include <hawk/View_group.h>
#include <hawk/handlers/List_dir.h>
#include "commands.h"
#include "Application.h"
#include "Dir_view.h"
#include "io.h"
#include "utils.h"

using namespace hawk;
using namespace std;

namespace {

View_group* get_active_vg(Application& app)
{
	return app.get_tab_manager().get_active()->get_view_group(1);
}

Dir_view_active& get_active_dv(View_group* vg)
{
	return static_cast<Dir_view_active&>(*(vg->get_views().back()));
}

bool is_num(const string& arg)
{
	return all_of(arg.begin(), arg.end(), [](char c) {
		return isdigit(c);
	});
}

string::size_type next_pos(string::size_type pos)
{
	return (pos == string::npos) ? pos : pos + 1;
}

template <typename String>
vector<String> split(const string& arg, char token)
{
	vector<String> v;
	string::size_type pos = 0;

	do {
		string::size_type nxpos = arg.find(token, pos);
		v.emplace_back(arg, pos, nxpos);
		pos = next_pos(nxpos);

	} while (pos != string::npos);

	return v;
}

Path get_active_cursor_path(Application& app)
{
	return get_active_dv(get_active_vg(app)).get_cursor_path();
}

bool parse_io_input(const string& arg, Application& app,
					vector<Path>& srcs, Path& dst)
{
	if (arg.empty()) throw invalid_argument {"empty argument"};

	View_group* vg = get_active_vg(app);

	auto dst_srcs = split<string>(arg, ':');
	Path base = vg->get_path();

	if (dst_srcs[0][0] == '/')
		dst = dst_srcs[0];
	else
		dst = base / dst_srcs[0];

	if (dst_srcs.size() == 1)
	{
		string src = get_active_cursor_path(app).string();
		if (src.empty())
		{
			cout << "Loading..." << endl;
			return false;
		}

		dst_srcs.push_back(src);
	}

	srcs = split<Path>(dst_srcs[1], ',');
	for (Path& src : srcs)
	{
		if (!src.is_absolute())
			src = base / src;
	}

	return true;
}

IO_task& parse_io_task(const string& arg, Application& app)
{
	if (!is_num(arg)) throw invalid_argument {arg};
	int id = atoi(arg.c_str());

	return app.get_io_manager().get(id);
}

void print_setting(const char* setting, bool val)
{
	cout << setting;
	cout << " is " << ((val) ? "ON" : "OFF") << endl;
}

void advance_cursor(Application& app, int n, List_dir::Cursor_position zero_pos)
{
	View_group* vg = get_active_vg(app);

	if (n == 0) vg->rewind_cursor(zero_pos);
	else vg->advance_cursor(n);
}

} // unnamed-namespace

// Change Directory command

void Cmd_cd::print_prompt() const
{
	cout << "cd " << flush;
}

void Cmd_cd::execute(const std::string& arg)
{
	View_group* vg = get_active_vg(m_app);

	if (arg[0] == '/')
		vg->set_path(arg);
	else if (arg == "*")
		vg->set_path(vg->get_cursor_path());
	else
		vg->set_path(vg->get_path() / arg);
}

// Change Cursor command

void Cmd_cc::print_prompt() const
{
	cout << "cc " << flush;
}

void Cmd_cc::execute(const string& arg)
{
	View_group* vg = get_active_vg(m_app);
	vg->set_cursor(arg, List_dir::Cursor_search_direction::detect);
}

// List Directory command

void Cmd_list::execute(const string&)
{
	const Dir_view& dv = get_active_dv(get_active_vg(m_app));
	dv.print_full();
}

// Column-List Directories command

void Cmd_column_list::execute(const string& arg)
{
	if (!arg.empty()) throw invalid_argument {arg};

	View_group* vg = get_active_vg(m_app);

	vector<vector<string>> cols;

	for (const auto& dv : vg->get_views())
	{
		cols.emplace_back(static_cast<const Dir_view*>(
							  dv.get())->get_column_list());
	}

	unsigned ncols = cols.size();
	unsigned nrows = 0;

	for (const auto& v : cols)
	{
		if (v.size() > nrows)
			nrows = v.size();
	}

	for (unsigned i = 0; i < nrows; i++)
	{
		for (unsigned j = 0; j < ncols; j++)
		{
			const auto& col = cols[j];

			if (col.size() > i)
				cout << col[i] << '\t';
			else
				cout << "\t\t";
		}

		cout << '\n';
	}

	cout << vg->get_path().c_str() << endl;
}

// Cursor up command

void Cmd_cursor_up::execute(const std::string& arg)
{
	if (!is_num(arg)) throw invalid_argument {arg};
	int n = ((arg.empty()) ? 1 : atoi(arg.c_str())) * -1;

	advance_cursor(m_app, n, List_dir::Cursor_position::beg);
}

// Cursor down command

void Cmd_cursor_down::execute(const std::string& arg)
{
	if (!is_num(arg)) throw invalid_argument {arg};
	int n = (arg.empty()) ? 1 : atoi(arg.c_str());

	advance_cursor(m_app, n, List_dir::Cursor_position::end);
}

// Cursor begin command

void Cmd_cursor_begin::execute(const std::string& arg)
{
	if (!arg.empty()) throw invalid_argument {arg};
	advance_cursor(m_app, 0, List_dir::Cursor_position::beg);
}


void Cmd_cursor_end::execute(const std::string& arg)
{
	if (!arg.empty()) throw invalid_argument {arg};
	advance_cursor(m_app, 0, List_dir::Cursor_position::end);
}

// Copy command

void Cmd_cp::print_prompt() const
{
	cout << "cp " << flush;
}

void Cmd_cp::execute(const std::string& arg)
{
	if (settings(arg.c_str()))
		return;

	vector<Path> srcs;
	Path dst;

	if (parse_io_input(arg, m_app, srcs, dst))
		m_app.get_io_manager().add<IO_copy>(srcs, dst, deref, update);
}

bool Cmd_cp::settings(const char* arg)
{
	if (*arg != ':')
		return false;

	arg++;

	if (strcmp(arg, "show") == 0)
	{
		print_setting("update", update);
		print_setting("deref", deref);

		return true;
	}

	bool val;
	if (strcmp(arg, "update") == 0)
		val = update = !update;
	else if (strcmp(arg, "deref") == 0)
		val = deref = !deref;
	else
		throw invalid_argument {arg};

	print_setting(arg, val);

	return true;
}

// Move command

void Cmd_mv::print_prompt() const
{
	cout << "mv " << flush;
}

void Cmd_mv::execute(const std::string& arg)
{
	if (settings(arg.c_str()))
		return;

	vector<Path> srcs;
	Path dst;

	if (parse_io_input(arg, m_app, srcs, dst))
		m_app.get_io_manager().add<IO_move>(srcs, dst, update);
}

bool Cmd_mv::settings(const char* arg)
{
	if (*arg != ':')
		return false;

	arg++;

	if (strcmp(arg, "show") == 0)
	{
		print_setting("update", update);
		return true;
	}

	if (strcmp(arg, "update") == 0)
		update = !update;
	else
		throw invalid_argument {arg};

	print_setting(arg, update);

	return true;
}

// Remove command

void Cmd_rm::execute(const std::string& arg)
{
	if (arg.empty()) throw invalid_argument {"empty argument"};

	Path base = get_active_vg(m_app)->get_path();
	auto list = split<Path>(arg, ',');

	for (Path& p : list)
	{
		if (!p.is_absolute())
			p = base / p;
	}

	m_app.get_io_manager().add<IO_remove>(list);
}

// Pause IO task command

void Cmd_io_pause::execute(const std::string& arg)
{
	parse_io_task(arg, m_app).pause();
}

// Resume IO task command

void Cmd_io_resume::execute(const std::string& arg)
{
	parse_io_task(arg, m_app).resume();
}

// Cancel IO task command

void Cmd_io_cancel::execute(const std::string& arg)
{
	parse_io_task(arg, m_app).cancel();
}

// Create tab command

void Cmd_create_tab::execute(const std::string& arg)
{
	View_group* vg = get_active_vg(m_app);
	Path p = (arg.empty()) ? get_active_vg(m_app)->get_path() : arg;

	if (!p.is_absolute())
		p = vg->get_path() / arg;

	m_app.get_tab_manager().add(p, m_app.get_cursor_cache());
}

// Remove tab command

void Cmd_remove_tab::execute(const std::string& arg)
{
	if (!is_num(arg)) throw invalid_argument {arg};

	Tab_manager& tm = m_app.get_tab_manager();
	unsigned id = (arg.empty())
			? tm.get_active()->get_id()
			: atoi(arg.c_str());

	tm.remove(id);
}

// Switch tab command

void Cmd_switch_tab::execute(const std::string& arg)
{
	if (arg.empty()) throw invalid_argument {"empty argument"};
	if (!is_num(arg)) throw invalid_argument {arg};

	unsigned id = atoi(arg.c_str());
	if (id == 0) throw invalid_argument {arg};

	m_app.get_tab_manager().set_active(id);
}

// List tabs command

void Cmd_list_tabs::execute(const std::string& arg)
{
	if (!arg.empty()) throw invalid_argument {arg};

	auto& tabs = m_app.get_tab_manager().get_container();
	for (auto& tp : tabs)
	{
		Tab& t = tp.second;
		cout << " T" << t.get_id() << ": ";
		cout << t.get_view_group(1)->get_path() << '\n';
	}

	cout << tabs.size() << " tabs listed" << endl;
}
