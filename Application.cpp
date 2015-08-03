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
#include <hawk/User_data.h>
#include <hawk/Filesystem.h>
#include <hawk/handlers/List_dir_hash.h>
#include "utils.h"
#include "Application.h"
#include "Entry_info.h"
#include "Dir_view.h"

using namespace hawk;
using namespace std;

namespace {

void on_fs_change(Tab_manager::Container& tabs, const vector<size_t>& hvec)
{
	for (auto& t_p : tabs)
	{
		Tab& t = t_p.second;
		auto& vgroups = t.get_container();

		auto vg_it = find_if(vgroups.begin(), vgroups.end(),
			[&](const auto& vg_p) {
				const View_group* vg = vg_p.second.get();
				const View_group::List_dir_vector& views = vg->get_views();

				auto view_it = find_if(views.begin(), views.end(),
					[&](const View_group::List_dir_ptr& v) {
						size_t h = v->get_path().hash();
						auto h_it = find(hvec.begin(), hvec.end(), h);

						return h_it != hvec.end();
					});

				return view_it != views.end();
			});

		if (vg_it != vgroups.end())
			vg_it->second->reload_path();
	}
}

void on_sort_change(Id_store<Tab>::Container_type& tabs)
{
	for (auto& t : tabs)
	{
		auto& vgroups = t.second.get_container();
		for (auto& v : vgroups)
			v.second->reload_path();
	}
}

bool sort_predicate(const Dir_entry& a, const Dir_entry& b)
{
	const Entry_info* ai = user_data_cast<const Entry_info>(&a.user_data);
	const Entry_info* bi = user_data_cast<const Entry_info>(&b.user_data);
	assert(ai && bi);

	char ta = tolower(ai->type);
	char tb = tolower(bi->type);

	if (ta == 'd' && tb != 'd')
		return true;

	if (ta != 'd' && tb == 'd')
		return false;

	return a.path.icompare(b.path) < 0;
}

void populate(const Path& p, User_data& ud)
{
	Entry_info i;
	i.st = symlink_status(p);
	i.tag = ' ';

	if (is_directory(i.st)) i.type = 'd';
	else if (is_symlink(i.st)) i.type = '@';
	else i.type = 'f';

	ud = i;
}

} // unnamed-namespace

Application::Application(const Path& init_path) : m_tm{m_vt}
{
	register_handlers();

	set_on_sort_change([&]{ on_sort_change(m_tm.get_container()); });
	set_populate_user_data(&populate);
	set_sort_predicate(&sort_predicate);

	using namespace std::chrono_literals;
	start_filesystem_watchdog(50ms, [&](const vector<size_t>& hvec){
		on_fs_change(m_tm.get_container(), hvec);
	});

	m_tm.add(init_path, m_cc);
}

const View_types& Application::get_type_factory()
{
	return m_vt;
}

Cursor_cache& Application::get_cursor_cache()
{
	return m_cc;
}

Tab_manager& Application::get_tab_manager()
{
	return m_tm;
}

IO_manager& Application::get_io_manager()
{
	return m_iom;
}

void Application::register_handlers()
{
	m_vt.register_type(hash_list_dir(), [&]{ return new Dir_view {m_cc}; });
}
