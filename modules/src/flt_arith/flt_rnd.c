/* Rounding to the precision of a target type. */

#include <stdint.h>
#include "flt_misc.h"

/* A 64-bit value as two 32-bit halves: the code has to work where there is no
   64-bit type, as in a compiler for a 16-bit machine. */

static uint32_t
lowbits(int n)
{
	/* the mask of the n lowest bits, 0 <= n < 32 */
	return n == 0 ? 0 : (uint32_t)0xFFFFFFFF >> (32 - n);
}

void
flt_rnd(flt_arith *e, int bits, int emin)
{
	/*	Round e to bits bits, to nearest, ties to even; below 2^emin
		fewer bits are kept, as in a subnormal number.
		The value is m * 2^(flt_exp - 63), m the 64-bit mantissa.
	*/
	uint32_t mh = e->m1, ml = e->m2;
	uint32_t kh, kl;	/* what is kept */
	uint32_t rh, rl;	/* what is rounded off */
	uint32_t hh, hl;	/* half of a unit */
	int drop;

	if (mh == 0 && ml == 0)
		return;
	if (e->flt_exp < emin)
		bits -= emin - e->flt_exp;
	drop = 64 - bits;	/* the number of bits to round off */
	if (drop <= 0)
		return;
	if (drop > 64) {
		/* less than half the smallest unit */
		kh = kl = 0;
	}
	else if (drop == 64) {
		/* m is at least half a unit; exactly half rounds to 0 */
		kh = 0;
		kl = mh > 0x80000000UL || (mh == 0x80000000UL && ml != 0);
	}
	else {
		if (drop >= 32) {
			kh = 0;
			kl = drop == 32 ? mh : mh >> (drop - 32);
			rh = mh & lowbits(drop - 32);
			rl = ml;
			hh = drop == 32 ? 0 : (uint32_t)1 << (drop - 33);
			hl = drop == 32 ? 0x80000000UL : 0;
		}
		else {
			kh = mh >> drop;
			kl = (ml >> drop) | (mh << (32 - drop));
			rh = 0;
			rl = ml & lowbits(drop);
			hh = 0;
			hl = (uint32_t)1 << (drop - 1);
		}
		if (rh > hh || (rh == hh && (rl > hl || (rl == hl && (kl & 1))))) {
			kl++;
			if (kl == 0)
				kh++;
		}
	}
	if (kh == 0 && kl == 0) {
		e->m1 = e->m2 = 0;
		e->flt_exp = 0;
		e->flt_sign = 0;
		return;
	}
	/* keep units of 2^(flt_exp - 63 + drop): normalize again */
	e->m1 = kh;
	e->m2 = kl;
	e->flt_exp += drop;
	flt_nrm(e);
	flt_chk(e);
}
