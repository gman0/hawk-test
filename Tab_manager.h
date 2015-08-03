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

#ifndef _tab_manager_h_
#define _tab_manager_h_

#include <hawk/Path.h>
#include <hawk/View_types.h>
#include "Id_store.h"
#include "Tab.h"

class Tab_manager
{
public:
	using Container = Id_store<Tab>::Container_type;

private:
	const hawk::View_types& m_vt;

	Id_store<Tab> m_tabs;
	Tab* m_active;

public:
	Tab_manager(const hawk::View_types& vt) : m_vt{vt}, m_active{nullptr} {}

	unsigned add(const hawk::Path& p, hawk::Cursor_cache& cc);
	bool remove(unsigned id);
	Tab& get(unsigned id);

	Tab* get_active() const;
	void set_active(unsigned id);

	Container& get_container();
};

#endif // _tab_manager_h_
