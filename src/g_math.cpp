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
//  src/g_math.cpp
//----------------------------------------------------------
#include "n_shared.h"
#include "scf.h"
#include "g_zone.h"
#include "g_items.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_scripted.h"
#include "s_world.h"
#include "g_playr.h"
#include "g_game.h"

nomadbool_t inArea(const area_t& a, const coord_t& pos)
{
	return (pos.y >= a.tl.y && pos.y <= a.br.y) && (pos.x >= a.tl.x && pos.x <= a.br.x);
}


//
// G_CastRay(): the general-use function that casts a "ray" from a line or slope,
// and determines what it first collides with. This is really just a hitscan collider,
// but I couldn't think of a better name for it.
//
// returns if it hit a wall, and another function (ideally the shooting function) will be
// calling it in a for every bullet in a shot loop. If it hits an entity, it deals damage to
// that entity
//
collider_t G_CastRay(coord_t endpoint, coord_t startpoint, Game* const game)
{
	nomadshort_t dx = endpoint.x > startpoint.x ? endpoint.x - startpoint.x : startpoint.x - endpoint.x;
	nomadshort_t sx = endpoint.x > startpoint.x ? 1 : -1;
	nomadshort_t dy = endpoint.y > startpoint.y ? endpoint.y - startpoint.y : startpoint.y - endpoint.y; 
	nomadshort_t sy = endpoint.y > startpoint.y ? 1 : -1;
	nomadshort_t err = (dx > dy ? dx : -dy) >> 1;
	collider_t collider;
	while (1) {
		if (G_CheckCollider(startpoint, game, collider)) break;
		if (startpoint.x == endpoint.x && startpoint.y == endpoint.y) break;
		nomadshort_t e2 = err;
		if (e2 > -dx) {
			err -= dy;
			startpoint.x += sx;
		}
		if (e2 < dy) {
			err += dx;
			startpoint.y += sy;
		}
	}
	return collider;
}

#include <bit>
#include <limits.h>

/*
* this algorithm was improved over the many months of this game's development.
* originally, it didn't compute distance between diagonal objects, only vertical and horizontal
* but now, thanks to my friend Catazat, it does, using the pythagorean theorem.
*
* note: some small optimizations have been made to the release mode
*
* another note: I thought up this algo in math class, freshmen year, when i should have been doing algebra,
* but i have zero regrets
*/
nomadlong_t disBetweenOBJ(const coord_t& src, const coord_t& tar)
{
	if (src.y == tar.y) { // horizontal
		return src.x > tar.x ? (src.x - tar.x) : (tar.x - src.x);
	}
	else if (src.x == tar.x) { // vertical
		return src.y > tar.y ? (src.y - tar.y) : (tar.y - src.y);
	}
	else { // diagonal
		return abs(Q_root((pow((src.x - tar.x), 2) + pow((src.y - tar.y), 2))));
	}
}

// NOTE: some of this algo was generated by ChatGPT, although heavily modified and changed up by me
coord_t closestOBJ(const std::vector<coord_t>& coords, const coord_t src)
{
	// Initialize minimum distance to a large number
	nomaduint_t mindis = std::numeric_limits<nomaduint_t>::max();
	coord_t closest;
	// Iterate through the vector and find the closest coordinates
	for (const coord_t& coord : coords) {
		nomaduint_t dis = disBetweenOBJ(coord, src);
		if (dis < mindis) {
			mindis = dis;
			closest = coord;
		}
	}
	return closest;
}

static constexpr float threehalfs = 1.5f;

// inspired heavly from Q3 Fast Inverse Square Root algorithm
// quick square root, because normal sqrts are too slow for me
inline nomadfloat_t Q_root(nomadfloat_t x)
{
	nomadlong_t        i;								// The integer interpretation of x
	nomadfloat_t       x_half = x * 0.5f;
	nomadfloat_t       r_sqrt = x;
/*#ifdef _NOMAD_DEBUG
	if (!(x > 0) || std::isnan(x) || std::isinf(x))
		return std::numeric_limits<nomadfloat_t>::quiet_NaN();
	
	i = *reinterpret_cast<int64_t*>(&r_sqrt);
	i = 0x5f375a86 - (i >> 1);
	r_sqrt = *reinterpret_cast<nomadfloat_t*>(&i);
#else*/
	// trick c/c++, bit hack
	i = *(nomadlong_t *)&r_sqrt;					    // oh yes, undefined behaviour, who gives a fuck?
	i = 0x5f375a86 - (i >> 1);				            // weird magic base-16 nums
	r_sqrt = *(nomadfloat_t *) &i;
//#endif
	r_sqrt = r_sqrt * (threehalfs - (x_half * r_sqrt * r_sqrt)); // 1st Newton iteration
	r_sqrt = r_sqrt * (threehalfs - (x_half * r_sqrt * r_sqrt)); // 2nd Newton iteration

	return x * r_sqrt; // x * (1/sqrt(x)) := sqrt(x)
}
