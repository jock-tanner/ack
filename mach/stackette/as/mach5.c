#include <stdlib.h>
#define RCSID5 "$Id$"

/*
 * Stackette: building the instructions
 */


#define FMT_IMM8	0
#define FMT_PFP		1
#define FMT_GEN		2

static void emit_extra(expr_t e)
{
#ifdef RELOCATION
	newrelo(e.typ, RELO2);
#endif
	emit2((int)e.val);
}

/*
 * The word offset of [pfp+n]: n is a byte offset, even, -128..126
 */
static int pfp_field(int n)
{
	if (n & 1 || n < -128 || n > 126) {
		serror("[pfp+n]: n is even and -128..126");
		return 0;
	}
	return (n >> 1) & 0x7f;
}

/*
 * In the frame of a variadic function the parameter count is at [pfp+0], so
 * the named parameters (non-negative offsets) are two bytes higher.
 */
static int par_off(int n)
{
	return frm_var && n >= 0 ? n + 2 : n;
}

/*
 * ld and the ALU operations: `op` has the opcode in the low 4 bits and the
 * byte form in bit 4; `dd` is the stack part; the operand is in `opnd`.
 * st: the same, with the operand as the destination (`is_dst`).
 */
void emit_gen(int op, int dd, int is_dst)
{
	int s = (op >> 4) & 1;
	int o = op & 15;
	int w;
	int f;

	switch (opnd.kind) {
	case K_IMM8:
		if (is_dst || s) {
			serror("an 8-bit immediate is a word source");
			return;
		}
		if ((o >= 4 && o < 8) || o == 13 ? (opnd.field < 0 || opnd.field > 255)
				    : (opnd.field < -128 || opnd.field > 127)) {
			serror("8-bit immediate out of range");
			return;
		}
		emit2((o << 10) | (dd << 8) | (opnd.field & 0xff));
		return;
	case K_PFP:
		w = (FMT_PFP << 14) | (o << 10) | (dd << 8) | (s << 7) |
			pfp_field(par_off(opnd.field));
		emit2(w);
		return;
	case K_REG:
		f = opnd.field;
		break;
	case K_PTR:
		f = 0x16;
		break;
	case K_SP:
		f = 0x40 | (opnd.field << 1);
		break;
	case K_IDX:
		f = 0x10 | (opnd.field << 1);
		break;
	case K_ABS:
		f = 0x20;
		break;
	case K_IMM16:
		if (is_dst) {
			serror("cannot store to an immediate");
			return;
		}
		f = 0x22;
		break;
	default:
		serror("bad operand");
		return;
	}
	w = (FMT_GEN << 14) | (o << 10) | (dd << 8) | (s << 7) | f;
	emit2(w);
	if (opnd.kind == K_ABS || opnd.kind == K_IMM16)
		emit_extra(opnd.extra);
}

/*
 * lea dd, [pfp+n]
 */
void emit_lea(int dd)
{
	emit2((FMT_PFP << 14) | (OP_LEA << 10) | (dd << 8) |
		pfp_field(par_off(opnd.field)));
}

/*
 * bra and call: an 11-bit word offset from the next instruction, to a label
 * of the same section. A bra that does not reach becomes a jmp, which takes
 * a second word: small() decides that over the passes.
 */
void emit_branch(int word, expr_t e)
{
	long off;
	int fits;

	off = (long)e.val - (long)(DOTVAL + 2);
	if (pass == PASS_2 && off > 0 && !(e.typ & S_DOT))
		off -= DOTGAIN;
	fits = !(off & 1) && off >= -2048 && off <= 2046;
	if ((e.typ & ~S_DOT) != (DOTTYP & ~S_DOT) && (e.typ & S_TYP) != S_UND)
		fits = 0;
	if ((word & 0xffff) != 0xc000) {
		if (!fits && pass == PASS_3)
			serror("call target out of range or odd");
		emit2(word | (int)((off >> 1) & 0x7ff));
		return;
	}
	if (small(fits, 2)) {
		emit2(word | (int)((off >> 1) & 0x7ff));
		return;
	}
	emit2(0xf980);
#ifdef RELOCATION
	newrelo(e.typ, RELO2);
#endif
	emit2(e.val);
}

/*
 * addp: the immediate is -128..127 bytes, bigger values take several
 * instructions
 */
void emit_addp(int base, int reg, int bytes)
{
	while (bytes > 126 || bytes < -128) {
		int part = bytes > 0 ? 126 : -128;

		emit2(base | ((reg - 2) << 8) | (part & 0xff));
		bytes -= part;
	}
	emit2(base | ((reg - 2) << 8) | (bytes & 0xff));
}

/*
 * The epilogue of a variadic function: the caller has put n = parameter
 * words - r at [pfp+0], and the result goes to [pfp+2n] through a pointer.
 * The variadic ret takes n from [pfp+0] too.
 */
static void emit_retv(int words)
{
	int i;

	if (words > 0) {
		emit2((FMT_PFP << 14) | (OP_LEA << 10) | (2 << 8) | pfp_field(0));
		emit2((FMT_PFP << 14) | (OP_LD << 10) | (2 << 8) | pfp_field(0));
		emit2((2 << 14) | (OP_ADD << 10) | (0 << 8) | 0x40);	/* add tos, tos */
		emit2((2 << 14) | (OP_ADD << 10) | (2 << 8) | 0x40);	/* add push, tos */
		for (i = 0; i < words; i++) {
			emit2((2 << 14) | (OP_ST << 10) | (3 << 8) | 0x16);
			if (i < words - 1)
				emit2((OP_ADD << 10) | 2);
		}
		emit2((2 << 14) | (OP_ST << 10) | (2 << 8) | 0);	/* st null, pop */
	}
	if (frm_loc)
		emit_addp(0xd400, 4, frm_loc);
	emit2(0xfa40);
}

/*
 * retf $r: the epilogue of a function that returns r words on the stack.
 * The result goes to [pfp+2n], n = parameter words - r, the locals are
 * released, and ret #n drops the rest of the parameters. If the result is
 * bigger than the parameters, it goes below pfp, and ret #0 leaves it there.
 */
/*
 * shl, shr and sar with a count: kind 0 shl, 1 shr, 2 sar (anything else is
 * not a shift with a count); the byte form has bit 4 set in `op`. Either an
 * immediate count 0..15 and dd tos or nos, or the count in TOS (`tos` as the
 * second operand, var = 1 + register number, so only tos is 1) with dd push.
 */
void emit_shift(int kind, int op, int dd, int count, int var)
{
	int v = var != 0;

	if (kind < 0 || kind > 2) {
		serror("only shl, shr and sar have a count");
		return;
	}
	if (v) {
		if (var != 1 || dd != 2)
			serror("shift by a count in tos: push, tos");
		dd = 0;
		count = 0;
	} else {
		if (dd > 1)
			serror("shift by a constant: tos or nos");
		if (count < 0 || count > 15)
			serror("shift count 0..15");
	}
	emit2(0xe800 | kind << 8 | (op & 16) << 3 | v << 6 | (dd & 3) << 4 |
		(count & 15));
}

void emit_retf(int words)
{
	int n = frm_par / 2 - words;
	int base = n >= 0 ? 2 * n : 2 * n;
	int free_bytes = frm_loc;
	int i;

	if (frm_var) {
		emit_retv(words);
		return;
	}
	if (n < 0)
		free_bytes -= 2 * words - frm_par;
	if (frm_par + frm_loc > 0 && frm_par + frm_loc < 2 * words) {
		/*
		 * The result moves up by less than its size, so popping the
		 * words from the lowest would overwrite the higher ones before
		 * they are read. Copy from the highest, leaving the stack
		 * alone, and then release it all.
		 */
		int src = -frm_loc - 2 * words;

		for (i = words - 1; i >= 0; i--) {
			emit2((FMT_PFP << 14) | (OP_LD << 10) | (2 << 8) |
				pfp_field(src + 2 * i));
			emit2((FMT_PFP << 14) | (OP_ST << 10) | (2 << 8) |
				pfp_field(base + 2 * i));
		}
		emit_addp(0xd400, 4, 2 * words + free_bytes);
	} else {
		for (i = 0; i < words; i++)
			emit2((FMT_PFP << 14) | (OP_ST << 10) | (2 << 8) |
				pfp_field(base + 2 * i));
		if (free_bytes)
			emit_addp(0xd400, 4, free_bytes);
	}
	if (n < 0)
		n = 0;
	if (n > 63) {
		serror("retf: too many parameter words");
		return;
	}
	emit2(0xf840 | n);
}
