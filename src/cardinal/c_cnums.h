#ifndef _C_CNUMS_
#define _C_CNUMS_

#include "../n_shared.h"

__uint64_t table_size;
std::vector<int32_t> cnum_table;

int32_t gen_cnum(void)
{
	return (rand() % 0x777777f);
}

static constexpr uint32_t seed = 0xfd341ce;
static constexpr __uint128_t magic = 0x373fea6341;

void __attribute__((constructor)) cnum_gentable(void)
{
	srand(seed ^ (time(nullptr)));
	table_size = rand() % magic;
	for (__uint128_t i = 0; i < table_size; i++) {
		int32_t cnum = gen_cnum();
		cnum_table.push_back(cnum);
	}
}

#endif