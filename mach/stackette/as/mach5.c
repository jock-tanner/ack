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
			pfp_field(opnd.field);
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
		pfp_field(opnd.field));
}

/*
 * bra and call: an 11-bit word offset from the next instruction, to a label
 * of the same section
 */
void emit_branch(int word, expr_t e)
{
	long off;

	if (pass == PASS_1) {
		emit2(word);
		return;
	}
	if ((e.typ & ~S_DOT) != (DOTTYP & ~S_DOT) && (e.typ & S_TYP) != S_UND) {
		serror("branch to another section");
		emit2(word);
		return;
	}
	off = (long)e.val - (long)(DOTVAL + 2);
	if (off & 1 || off < -2048 || off > 2046) {
		if (pass == PASS_3)
			serror("branch target out of range or odd");
		off = 0;
	}
	emit2(word | (int)((off >> 1) & 0x7ff));
}

/*
 * retf $r: the epilogue of a function that returns r words on the stack.
 * The result goes to [pfp+2n], n = parameter words - r, the locals are
 * released, and ret #n drops the rest of the parameters. If the result is
 * bigger than the parameters, it goes below pfp, and ret #0 leaves it there.
 */
void emit_retf(int words)
{
	int n = frm_par / 2 - words;
	int base = n >= 0 ? 2 * n : 2 * n;
	int free_bytes = frm_loc;
	int i;

	if (n < 0)
		free_bytes -= 2 * words - frm_par;
	for (i = 0; i < words; i++)
		emit2((FMT_PFP << 14) | (OP_ST << 10) | (2 << 8) |
			pfp_field(base + 2 * i));
	if (free_bytes < -128 || free_bytes > 127) {
		serror("retf: frame too big");
		return;
	}
	if (free_bytes)
		emit2(0xd400 | (2 << 8) | (free_bytes & 0xff));
	if (n < 0)
		n = 0;
	if (n > 63) {
		serror("retf: too many parameter words");
		return;
	}
	emit2(0xf840 | n);
}
