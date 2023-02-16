#ifndef _G_CONSOLE_
#define _G_CONSOLE_

#pragma once

typedef struct _console_var
{
	const char* varstr;
	std::string value;
	bool isconst;
	void* varptr;
} cvar_t;

extern constexpr const char* commands[];
extern cvar_t cvars[];

#endif
