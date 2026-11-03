#define RCSID1 "$Id$"

/*
 * Stackette: the operand of a two-operand instruction, set by the rules of
 * the grammar and used by emit_gen()
 */
#define K_REG	0	/* a register: field */
#define K_IMM8	1	/* $expr: field is the value */
#define K_IMM16	2	/* #expr: the word in extra */
#define K_ABS	3	/* [expr]: the word in extra */
#define K_PFP	4	/* [pfp+expr]: field is the byte offset */
#define K_PTR	5	/* [tos] */
#define K_IDX	6	/* [mbp+midx], ++ after, -- before: field is 0, 1, 2 */

typedef struct {
	short	kind;
	short	field;
	expr_t	extra;
} opnd_t;

extern opnd_t opnd;

/* the opcodes of the two-operand instructions */
#define OP_LD	8
#define OP_ST	9
#define OP_LEA	11

void emit_gen(int op, int dd, int is_dst);
void emit_lea(int dd);
void emit_branch(int word, expr_t e);

/*
 * The frame of the current function, set by .frame: the bytes of locals and
 * of parameters. retf expands to the epilogue, see emit_retf().
 */
extern int frm_loc;
extern int frm_par;
void emit_retf(int words);
