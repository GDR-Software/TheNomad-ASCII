#include "n_shared.h"

/*
* this algorithm was improved over the many months of this game's development.
* originally, it didn't compute distance between diagonal objects, only vertical and horizontal
* but now, thanks to my friend Catazat, it does, using the pythagorean theorem.
*/
inline nomaduint_t disBetweenOBJ(coord_t src, coord_t tar)
{
	if (src.y == tar.y) { // horizontal
		if (src.x > tar.x) { return (src.x - tar.x);}
		else if (src.x < tar.x) { return (tar.x - src.x); }
		else { return 0; }
	}
	else if (src.x == tar.x) { // vertical
		if (src.y > tar.y) { return (src.y - tar.y); }
		else if (src.y < tar.y) { return (tar.y - src.y); }
		else { return 0 }
	}
	else { // diagonal
		return (pow(pow((src.x - tar.x), 2) - pow((src.y - tar.y), 2), 0.5));
	}
}

// inspired heavly from Q3 Fast Inverse Square Root algorithm
// quick square root, because normal sqrts are too slow for me
float Q_root(float x)
{
	long     i;								// The integer interpretation of x
	float    x_half = x * 0.5f;
	float    r_sqrt = x;

	// trick c/c++, bit hack
	i = *(long *)&r_sqrt;					// oh yes, undefined behaviour, who gives a fuck?
	i = 0x5f375a86 - (i >> 1);				// weird magic base-16 nums
	r_sqrt = *(float *) &i;

	r_sqrt = r_sqrt * (threehalfs - (x_half * r_sqrt * r_sqrt)); // 1st Newton iteration
	r_sqrt = r_sqrt * (threehalfs - (x_half * r_sqrt * r_sqrt)); // 2nd Newton iteration

	return x * r_sqrt; // x * (1/sqrt(x)) := sqrt(x)
}