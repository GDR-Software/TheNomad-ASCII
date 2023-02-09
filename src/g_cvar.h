#ifndef _G_CVAR_
#define _G_CVAR_

typedef struct _console_var
{
	const char* varstr;
	std::string value;
} cvar_t;

extern cvar_t cvars[];

#endif
