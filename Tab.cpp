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

#include <exception>
#include <iostream>
#include <hawk/Filesystem.h>
#include "Tab.h"
#include "Dir_view.h"

using namespace hawk;
using namespace std;

namespace {

void exception_handle(unsigned tab_id, std::exception_ptr eptr) noexcept
{
	try { rethrow_exception(eptr); }
	catch (const Filesystem_error& e) {
		cerr << "# T" << tab_id << " Filesystem_error: "
			 << e.what() << endl;
	}
}

} // unnamed-namespace

void Tab::set_id(unsigned id)
{
	m_id = id;
}

unsigned Tab::get_id() const
{
	return m_id;
}

unsigned Tab::add_view_group(const Path& p, const View_types& vt,
							 Cursor_cache& cc)
{
	auto eh = [id = m_id](std::exception_ptr eptr) {
		exception_handle(id, eptr);
	};

	View_types::Handler active_ld = [&]{ return new Dir_view_active {cc}; };

	using namespace chrono_literals;
	return m_vgroups.insert(
		std::make_unique<View_group>(
				p, 3, vt, cc, eh, active_ld, nullptr, 50ms));
}

bool Tab::remove_view_group(unsigned id)
{
	return m_vgroups.erase(id);
}

View_group* Tab::get_view_group(unsigned id)
{
	return m_vgroups[id].get();
}

Tab::Container& Tab::get_container()
{
	return m_vgroups.get_container();
}
