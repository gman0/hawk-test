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
