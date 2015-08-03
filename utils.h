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
