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
#include <hawk/handlers/List_dir_hash.h>
#include "Tab.h"
#include "Dir_view.h"

using namespace hawk;
using namespace std;

namespace {

void exception_handler(unsigned tab_id, std::exception_ptr eptr) noexcept
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
		exception_handler(id, eptr);
	};

	View_types::Handler active_ld = [&](View_group& parent){
		return new Dir_view_active {cc, parent};
	};
	auto ld = vt.get_handler(hash_list_dir());

	// Create a view group.
	auto vg = make_unique<View_group>(
				vt, cc, eh, chrono::milliseconds {50});

	// Add a couple of non-active views.
	for (int i = 0; i < 2; i++)
		vg->add_view(ld);

	// Add an active view.
	vg->add_view(active_ld);

	// Set the path. At this point, the view group is ready to use.
	vg->set_path(p);

	return m_vgroups.insert(std::move(vg));
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
