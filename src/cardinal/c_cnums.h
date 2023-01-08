#ifndef _C_CNUMS_
#define _C_CNUMS_

#include "../n_shared.h"
#include <limits.h>

typedef int64_t cnum;

namespace cardinal {

uint64_t table_size;
std::vector<cnum> cnum_table;

cnum get_cnum(void)
{
	return (rand() % table_size);
}

cnum gen_cnum(void)
{
	return (rand() % INT64_MAX);
}

static constexpr uint32_t seed = 0xfd341ce;
static constexpr __uint128_t magic = 0x373fea6341;

void cnum_gentable(void)
{
	srand(seed ^ (time(nullptr)));
	table_size = rand() % magic;
	cnum_table.reserve(table_size);
	for (__uint128_t i = 0; i < table_size; ++i) {
		cnum num = gen_cnum();
		cnum_table.emplace_back();
		cnum_table.back() = num;
	}
}

};

#endif