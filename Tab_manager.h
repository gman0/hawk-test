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
