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

#ifndef _application_h_
#define _application_h_

#include <hawk/View_types.h>
#include <hawk/Cursor_cache.h>
#include <hawk/Path.h>
#include "Id_store.h"
#include "Tab_manager.h"
#include "io.h"

class Application
{
private:
	hawk::View_types m_vt;
	hawk::Cursor_cache m_cc;

	Tab_manager m_tm;
	IO_manager m_iom;

public:
	explicit Application(const hawk::Path& init_path);

	const hawk::View_types& get_type_factory();
	hawk::Cursor_cache& get_cursor_cache();
	Tab_manager& get_tab_manager();
	IO_manager& get_io_manager();

private:
	void register_handlers();
};

#endif // _application_h_
