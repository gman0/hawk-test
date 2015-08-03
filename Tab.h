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
