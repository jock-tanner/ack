/*
 * Machine dependent back end routines for Stackette, a 16-bit little-endian
 * stack machine.
 */

#include <stdlib.h> /* atol */

/*
 * Words are 16 bits, and bytes within a word go to the lower address first
 */
void con_part(int sz, word w)
{

	while (part_size % sz)
		part_size++;
	if (part_size == 2)
		part_flush();
	if (sz == 1)
	{
		w &= 0xFF;
		if (part_size)
			w <<= 8;
		part_word |= w;
	}
	else
	{
		assert(sz == 2);
		part_word = w;
	}
	part_size += sz;
}

void
con_mult(word sz) {

	if (sz != 4 && sz != 8)
		fatal("bad icon/ucon size");
	fprintf(codefile,".data%d\t%s\n", (int)sz, str);
}

#define CODE_GENERATOR
#define IEEEFLOAT
#define FL_MSL_AT_LOW_ADDRESS 0
#define FL_MSW_AT_LOW_ADDRESS 0
#define FL_MSB_AT_LOW_ADDRESS 0
#include <con_float>

/*
 * The call instruction already made pfp point at the saved frame pointer, so
 * only the room for locals has to be reserved
 */
void prolog(full nlocals)
{
	if (nlocals > 0)
		fprintf(codefile, "\taddp sp, $-%ld\n", (long)nlocals);
}

void mes(word type)
{
	int argt;

	switch ((int)type)
	{
		case ms_ext:
			for (;;)
			{
				switch (argt = getarg(ptyp(sp_cend) | ptyp(sp_pnam) | sym_ptyp))
				{
					case sp_cend:
						return;
					default:
						strarg(argt);
						fprintf(codefile, ".define %s\n", argstr);
						break;
				}
			}
		default:
			while (getarg(any_ptyp) != sp_cend)
				;
			break;
	}
}

char* segname[] = { ".sect .text", ".sect .data", ".sect .rom", ".sect .bss" };
