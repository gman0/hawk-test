#ifndef _entry_info_h_
#define _entry_info_h_

#include <hawk/Filesystem.h>

struct Entry_info
{
	char tag; // '*' - current cursor; ' ' - others
	char type; // f - file; d - directory; @ - link
	hawk::Stat st;
};

#endif // _entry_info_h_

