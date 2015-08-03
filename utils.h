#ifndef _utils_h_
#define _utils_h_

#include <iostream>
#include <chrono>
#include <hawk/Path.h>

inline std::ostream& operator<<(std::ostream& os, const hawk::Path& p)
{
	os << '\'' << p.c_str() << '\'';
	return os;
}

template <typename Fn, typename... Args>
auto measure_time(Fn&& f, Args&&... args)
{
	auto start = std::chrono::steady_clock::now();
	f(std::forward<Args>(args)...);
	auto end = std::chrono::steady_clock::now();

	return std::chrono::duration_cast<std::chrono::milliseconds>(
				end - start).count();
}

#endif // _utils_h_
