#define RCSID4 "$Id$"

/*
 * Stackette parsing rules. The operands of the two-operand instructions are
 * collected in `opnd`, and emit_gen() builds the instruction.
 */
operation
	:	NOOPOP
			{	emit2($1);}
	|	ABSOP expr
			{	emit2($1);
#ifdef RELOCATION
				newrelo($2.typ, RELO2);
#endif
				emit2($2.val);
			}
	|	BRAOP expr
			{	emit_branch($1, $2);}
	|	BRAOP DDONLY
			{	if (($1 & 0xffff) != 0xf000 || $2 != 2)
					serror("only call pop");
				emit2(0xf9c0);
			}
	|	GENOP dd ',' src
			{	emit_gen($1, $2, 0);}
	|	STOP dest ',' dd
			{	emit_gen($1, $4, 1);}
	|	LEAOP dd ',' '[' PFP '+' absexp ']'
			{	opnd.kind = K_PFP; opnd.field = (short)$7;
				emit_lea($2);
			}
	|	LEAOP dd ',' '[' PFP '-' absexp ']'
			{	opnd.kind = K_PFP; opnd.field = (short)-$7;
				emit_lea($2);
			}
	|	SKIPOP COND
			{	emit2($1 | ($2 << 6));}
	|	ADDPOP reg ',' '$' absexp
			{	int r = (int)$2 == 1 ? 4 : (int)$2;	/* sp */

				if (r < 2 || r > 4)
					serror("addp: mbp, midx or sp");
				emit_addp((int)$1, r, (int)$5);
			}
	|	RETNOP '$' absexp
			{	if ($3 < 0 || $3 > 63)
					serror("ret: 0..63 words");
				emit2($1 | ($3 & 63));
			}
	|	FRAMEOP absexp ',' absexp
			{	frm_loc = (int)$2; frm_par = (int)$4; frm_var = (int)$1;}
	|	RETFOP '$' absexp
			{	emit_retf((int)$3);}
	|	BITOP '$' absexp
			{	if ($3 < 0 || $3 > 255)
					serror("setb/clrb: 0..255");
				emit2($1 | $3);
			}
	|	UNOP dd
			{	emit2(0xe000 | ($2 << 8) | (($1 & 16) << 3) |
					(($1 & 15) << 3));
			}
	|	SHLOP dd ',' '$' absexp
			{	emit_shift((int)$1, (int)$2, (int)$5, 0);}
	|	SHLOP dd ',' TOSNOS
			{	emit_shift((int)$1, (int)$2, 0, 1 + (int)$4);}
	|	GENDD src
			{	emit_gen($1 & 255, $1 >> 8, 0);}
	|	STDD dest
			{	emit_gen($1 & 255, $1 >> 8, 1);}
	|	LEADD '[' PFP '-' absexp ']'
			{	opnd.kind = K_PFP; opnd.field = (short)-$5;
				emit_lea($1);
			}
	|	LEADD '[' PFP '+' absexp ']'
			{	opnd.kind = K_PFP; opnd.field = (short)$5;
				emit_lea($1);
			}
	;
dd	:	TOSNOS
			{	$$ = $1;}
	|	DDONLY
			{	$$ = $1;}
	;
reg	:	TOSNOS
			{	$$ = 0x100 + $1;}	/* a stack item, not a register */
	|	REGONLY
			{	$$ = $1;}
	;
src	:	'$' absexp
			{	opnd.kind = K_IMM8; opnd.field = (short)$2;}
	|	'#' expr
			{	opnd.kind = K_IMM16; opnd.extra = $2;}
	|	dest
	;
dest	:	'[' expr ']'
			{	opnd.kind = K_ABS; opnd.extra = $2;}
	|	'[' PFP '+' absexp ']'
			{	opnd.kind = K_PFP; opnd.field = (short)$4;}
	|	'[' PFP '-' absexp ']'
			{	opnd.kind = K_PFP; opnd.field = (short)-$4;}
	|	'[' TOSNOS ']'
			{	opnd.kind = K_PTR; opnd.field = 0;
				if ($2 != 0) serror("only [tos] is a pointer");
			}
	|	'[' reg '+' absexp ']'
			{	opnd.kind = K_SP; opnd.field = (short)$4;
				if ($2 != 1) serror("only [sp+k]");
				if ($4 < 0 || $4 > 31) serror("[sp+k]: k is 0..31");
			}
	|	'[' reg '+' reg ']'
			{	opnd.kind = K_IDX; opnd.field = 0;
				if ($2 != 2 || $4 != 3) serror("use [mbp+midx]");
			}
	|	'[' reg '+' reg '+' '+' ']'
			{	opnd.kind = K_IDX; opnd.field = 1;
				if ($2 != 2 || $4 != 3) serror("use [mbp+midx++]");
			}
	|	'[' reg '+' reg '+' '+' ']' '*'
			{	opnd.kind = K_IDX; opnd.field = 5;
				if ($2 != 2 || $4 != 3) serror("use [mbp+midx++]*");
			}
	|	'[' reg '+' '-' '-' reg ']' '*'
			{	opnd.kind = K_IDX; opnd.field = 6;
				if ($2 != 2 || $6 != 3) serror("use [mbp+--midx]*");
			}
	|	'[' reg '+' '-' '-' reg ']'
			{	opnd.kind = K_IDX; opnd.field = 2;
				if ($2 != 2 || $6 != 3) serror("use [mbp+--midx]");
			}
	|	reg
			{	if ($1 >= 0x100) {	/* tos, nos: [sp+0], [sp+1] */
					opnd.kind = K_SP; opnd.field = (short)($1 - 0x100);
				} else {
					opnd.kind = K_REG; opnd.field = (short)$1;
				}
			}
	;
