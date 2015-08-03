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

#ifndef _io_h_
#define _io_h_

#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <iostream>
#include <utility>
#include <cassert>
#include <hawk/Path.h>
#include <hawk/IO_tasking.h>
#include "Id_store.h"

struct IO_base
{
	unsigned id = 0;
};

class IO_manager;

class IO_copy : public hawk::IO_task_copy, public IO_base
{
public:
	friend class IO_manager;

	IO_copy(const hawk::Path& src, const hawk::Path& dst,
			bool deref = false, bool update = false);
	IO_copy(const std::vector<hawk::Path>& srcs, const hawk::Path& dst,
			bool deref = false, bool update = false);

private:
	virtual void on_error(const hawk::IO_task_error& e) const noexcept;
	virtual void on_error(const hawk::Filesystem_error& e) const noexcept;

	virtual void on_status_change(hawk::IO_task::Status st) const noexcept;
};

class IO_move : public hawk::IO_task_move, public IO_base
{
public:
	friend class IO_manager;

	IO_move(const hawk::Path& src, const hawk::Path& dst, bool update = false);
	IO_move(const std::vector<hawk::Path>& srcs, const hawk::Path& dst,
			bool update = false);

private:
	virtual void on_error(const hawk::IO_task_error& e) const noexcept;
	virtual void on_error(const hawk::Filesystem_error& e) const noexcept;

	virtual void on_status_change(hawk::IO_task::Status st) const noexcept;
};

class IO_remove : public hawk::IO_task_remove, public IO_base
{
public:
	friend class IO_manager;

	IO_remove(const hawk::Path& p);
	IO_remove(const std::vector<hawk::Path>& pvec);

private:
	virtual void on_error(const hawk::IO_task_error& e) const noexcept;
	virtual void on_error(const hawk::Filesystem_error& e) const noexcept;

	virtual void on_status_change(hawk::IO_task::Status st) const noexcept;
};

class IO_manager
{
public:
	using Container = Id_store<std::unique_ptr<hawk::IO_task>>::Container_type;

private:
	Id_store<std::unique_ptr<hawk::IO_task>> m_tasks;

public:
	IO_manager();

	template <typename Task, typename ...Args, typename = std::enable_if_t<
				  std::is_base_of<IO_base, Task>::value>>
	unsigned add(Args&&... args)
	{
		remove_finished();
		unsigned id = m_tasks.insert(
					std::make_unique<Task>(std::forward<Args>(args)...));
		std::cout << "Added IO" << id << std::endl;

		Task* task = static_cast<Task*>(m_tasks[id].get());
		task->id = id;
		task->start_tasking();

		return id;
	}

	hawk::IO_task& get(unsigned id);
	bool remove(unsigned id);

private:
	void remove_finished();
};

#endif // _io_h_
