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
#include "Tab_manager.h"
#include "utils.h"

using namespace std;

namespace {

void print_set_active(Tab& t)
{
	cout << "Switched to T" << t.get_id() << ':';
	cout << t.get_view_group(1)->get_path() << endl;
}

} // unnamed-namespace

unsigned Tab_manager::add(const hawk::Path& p, hawk::Cursor_cache& cc)
{
	unsigned id = m_tabs.insert(Tab {});
	Tab& t = m_tabs[id];

	t.set_id(id);
	t.add_view_group(p, m_vt, cc);

	m_active = &t;

	cout << "Added T" << id << ':' << p << ", set to active" << endl;

	return id;
}

bool Tab_manager::remove(unsigned id)
{
	if (m_tabs.get_container().size() == 1)
	{
		cout << "Won't remove the last tab!" << endl;
		return true;
	}

	Tab& t = get(id);

	if (&t == m_active)
	{
		m_tabs.erase(id);
		m_active = &m_tabs.get_container().begin()->second;
		id = m_active->get_id();

		print_set_active(*m_active);

		return true;
	}

	return m_tabs.erase(id);
}

Tab& Tab_manager::get(unsigned id)
{
	return m_tabs[id];
}

Tab* Tab_manager::get_active() const
{
	return m_active;
}

void Tab_manager::set_active(unsigned id)
{
	m_active = &m_tabs[id];
	print_set_active(*m_active);
}

Tab_manager::Container& Tab_manager::get_container()
{
	return m_tabs.get_container();
}
