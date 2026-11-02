/*
 * Stackette: a 16-bit stack machine, little-endian, 16-bit instruction words.
 */
#define RCSID0 "$Id$"

#define THREE_PASS
#define LISTING
#define RELOCATION

#undef ALIGNWORD
#define ALIGNWORD 2
#undef ALIGNSECT
#define ALIGNSECT 2
