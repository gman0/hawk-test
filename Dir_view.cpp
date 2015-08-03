#include <iostream>
#include <sstream>
#include <algorithm>
#include <hawk/User_data.h>
#include "Dir_view.h"
#include "Entry_info.h"
#include "utils.h"

using namespace hawk;
using namespace std;

namespace {

Entry_info* get_entry_info(Dir_entry& ent)
{
	Entry_info* ei = user_data_cast<Entry_info>(&ent.user_data);
	assert(ei);

	return ei;
}

string ellipsize(string str)
{
	// I really don't care about non-ascii strings
	// right now as this is just a test...

	if (str.length() > 11)
	{
		string tmp;
		tmp = str.substr(0, 7);
		tmp += "...";
		tmp += str.substr(str.length() - 3, str.length());

		str = move(tmp);
	}
	else
	{
		for (int i = 13 - str.length(); i > 0; i--)
			str += ' ';
	}

	return str;
}

ostream& operator<<(ostream& os, const Dir_entry& ent)
{
	const Entry_info* ei = user_data_cast<const Entry_info>(&ent.user_data);
	assert(ei);

	os << ei->tag << ei->type << ' ' << ent.path.c_str();

	return os;
}

void fill_vector(vector<string>& entries, Dir_cursor beg, Dir_cursor end)
{
	for_each(beg, end, [&](const Dir_entry& ent) {
		ostringstream os;
		os << ent;
		entries.emplace_back(ellipsize(os.str()));
	});
}

bool is_loading(unique_lock<mutex>& lk)
{
	if (!lk.try_lock())
	{
		cout << "Loading..." << endl;
		return true;
	}

	return false;
}

bool no_dir_ptr(const Dir_ptr& ptr)
{
	return !ptr || ptr->empty();
}

} // unnamed-namespace

Dir_view::Dir_view(Cursor_cache& cc)
	: List_dir{cc}, m_lk{m_mtx, std::defer_lock}
{}

void Dir_view::print_full() const
{
	unique_lock<mutex> lk {m_mtx, defer_lock};
	if (is_loading(lk)) return;

	size_t items = 0;
	if (!no_dir_ptr(m_dir_ptr))
	{
		Entry_info* cursor_ei = get_entry_info(*m_cursor);
		cursor_ei->tag = '*';

		for (const Dir_entry& ent : *m_dir_ptr)
			cout << ent << '\n';

		cursor_ei->tag = ' ';
		items = m_dir_ptr->size();
	}

	cout << m_path.c_str() << ": ";
	cout << items << " items listed" << endl;
}

vector<string> Dir_view::get_column_list() const
{
	vector<string> entries;

	unique_lock<mutex> lk {m_mtx, defer_lock};
	if (is_loading(lk)) return entries;

	if (!no_dir_ptr(m_dir_ptr))
	{
		size_t up = min(distance(m_dir_ptr->begin(), m_cursor), 3L);
		size_t down = min(distance(m_cursor, m_dir_ptr->end()), 4L);

		auto up_iter = m_cursor;
		auto down_iter = m_cursor;

		advance(up_iter, -up);
		advance(down_iter, down);

		Entry_info* cursor_ei = get_entry_info(*m_cursor);
		cursor_ei->tag = '*';

		fill_vector(entries, up_iter, m_cursor);
		fill_vector(entries, m_cursor, down_iter);

		cursor_ei->tag = ' ';
	}
	else
		entries.emplace_back("  empty  ");

	return entries;
}

void Dir_view::ready() noexcept
{
	m_lk.unlock();
}

void Dir_view::not_ready() noexcept
{
	m_lk.lock();
}

// Dir_view_active impl.

Path Dir_view_active::get_cursor_path() const
{
	unique_lock<mutex> lk {m_mtx, defer_lock};
	return (lk.try_lock() && m_dir_ptr) ? m_cursor->path : Path {};
}

void Dir_view_active::ready() noexcept
{
	cout << " (" << m_path << ": ";

	if (!m_dir_ptr || m_cursor == m_dir_ptr->end())
		cout << "N/A";
	else
		cout << m_cursor->path.c_str();

	cout << ") " << flush;

	Dir_view::ready();
}
