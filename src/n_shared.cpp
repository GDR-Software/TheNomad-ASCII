//----------------------------------------------------------
//
// Copyright (C) SIGAAMDAD 2022-2023
//
// This source is available for distribution and/or modification
// only under the terms of the SACE Source Code License as
// published by SIGAAMDAD. All rights reserved
//
// The source is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of FITNESS FOR A PARTICLAR PURPOSE. See the SACE
// Source Code License for more details. If you, however do not
// want to use the SACE Source Code License, then you must use
// this source as if it were to be licensed under the GNU General
// Public License (GPL) version 2.0 or later as published by the
// Free Software Foundation.
//
// DESCRIPTION:
//  src/n_shared.cpp
//----------------------------------------------------------
#include "n_shared.h"

void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return ::operator new(size);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return ::operator new(size);
}

namespace std {
	size_t filelength(const char* filename)
	{
#ifdef __unix__
		struct stat fdata;
		if (stat(filename, &fdata) == -1) {
			return -1;
		}
		else {
			return (size_t)fdata.st_size;
		}
#else // don't really want to be constructing an object whenever we do this
		FILE* fp = std::fopen(filename, "r");
		if (fp == NULL) {
			return -1;
		}
		else {
			::std::fseek(fp, 0, SEEK_END);
			size_t fsize = ::std::ftell(fp);
			::std::fseek(fp, 0, SEEK_SET);
			return fsize;
		}
#endif
	}
	size_t filelength(const std::string& filename)
	{
#ifdef __unix__
		struct stat fdata;
		if (stat(filename.c_str(), &fdata) == -1) {
			return -1;
		}
		else {
			return (size_t)fdata.st_size;
		}
#else // don't really want to be constructing an object whenever we do this
		FILE* fp = ::std::fopen(filename.c_str(), "r");
		if (fp == NULL) {
			return -1;
		}
		else {
			::std::fseek(fp, 0, SEEK_END);
			size_t fsize = ::std::ftell(fp);
			::std::fseek(fp, 0, SEEK_SET);
			return fsize;
		}
#endif
	}
};
