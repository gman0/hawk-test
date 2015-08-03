#ifndef _h_dir_view_
#define _h_dir_view_

#include <mutex>
#include <vector>
#include <string>
#include <hawk/handlers/List_dir.h>
#include <hawk/Cursor_cache.h>

class Dir_view : public hawk::List_dir
{
protected:
	mutable std::mutex m_mtx;
	mutable std::unique_lock<std::mutex> m_lk;

public:
	Dir_view(hawk::Cursor_cache& cc);

	void print_full() const;
	std::vector<std::string> get_column_list() const;

protected:
	virtual void ready() noexcept;
	virtual void not_ready() noexcept;
};

class Dir_view_active : public Dir_view
{
public:
	using Dir_view::Dir_view;

	hawk::Path get_cursor_path() const;

private:
	virtual void ready() noexcept;
};

class Dir_preview : public Dir_view
{
public:
	using Dir_view::Dir_view;
};

#endif // _h_dir_view_g
