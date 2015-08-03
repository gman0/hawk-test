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
