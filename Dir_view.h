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

#ifndef _h_dir_view_
#define _h_dir_view_

#include <mutex>
#include <vector>
#include <string>
#include <hawk/handlers/List_dir.h>
#include <hawk/Cursor_cache.h>

class Dir_view : public hawk::List_dir
{
protected:
	mutable std::mutex m_mtx;
	mutable std::unique_lock<std::mutex> m_lk;

public:
	Dir_view(hawk::Cursor_cache& cc, hawk::View_group& parent);

	void print_full() const;
	std::vector<std::string> get_column_list() const;

protected:
	virtual void ready() noexcept;
	virtual void not_ready() noexcept;
};

class Dir_view_active : public Dir_view
{
public:
	using Dir_view::Dir_view;

	hawk::Path get_cursor_path() const;

private:
	virtual void ready() noexcept;
};

class Dir_preview : public Dir_view
{
public:
	using Dir_view::Dir_view;
};

#endif // _h_dir_view_g
