#define RCSID3 "$Id$"

/*
 * Stackette keywords: mnemonic, the instruction word
 */
{0,	NOOPOP,		0xf800,		"nop"},
{0,	NOOPOP,		0xfa40,		"ret"},
{0,	NOOPOP,		0xf880,		"reti"},
{0,	ABSOP,		0xf940,		"calla"},
{0,	ABSOP,		0xf980,		"jmp"},
{0,	ABSOP,		0xa222,		"pushi"},
{0,	ABSOP,		0xa220,		"pushm"},
{0,	ABSOP,		0xa620,		"popm"},
