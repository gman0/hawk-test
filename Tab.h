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

#ifndef _tab_h_
#define _tab_h_

#include <memory>
#include <hawk/View_group.h>
#include "Id_store.h"

class Tab
{
public:
	using Container =
		Id_store<std::unique_ptr<hawk::View_group>>::Container_type;

private:
	unsigned m_id;

	// There is no point in using multiple view groups
	// in this setup (testing purposes only).
	Id_store<std::unique_ptr<hawk::View_group>> m_vgroups;

public:
	void set_id(unsigned id);
	unsigned get_id() const;

	unsigned add_view_group(const hawk::Path& p, const hawk::View_types& vt,
							hawk::Cursor_cache& cc);
	bool remove_view_group(unsigned id);
	hawk::View_group* get_view_group(unsigned id);

	Container& get_container();
};

#endif // _tab_h_
