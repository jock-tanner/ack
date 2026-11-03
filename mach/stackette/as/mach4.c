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
			{	if ($2 < 2 || $2 > 4)
					serror("addp: mbp, midx or sp");
				if ($5 < -128 || $5 > 127)
					serror("addp: -128..127");
				emit2($1 | (($2 - 2) << 8) | ($5 & 0xff));
			}
	|	RETNOP '$' absexp
			{	if ($3 < 0 || $3 > 63)
					serror("ret: 0..63 words");
				emit2($1 | ($3 & 63));
			}
	|	FRAMEOP absexp ',' absexp
			{	frm_loc = (int)$2; frm_par = (int)$4;}
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
			{	$$ = $1;}
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
	|	'[' reg '+' reg ']'
			{	opnd.kind = K_IDX; opnd.field = 0;
				if ($2 != 2 || $4 != 3) serror("use [mbp+midx]");
			}
	|	'[' reg '+' reg '+' '+' ']'
			{	opnd.kind = K_IDX; opnd.field = 1;
				if ($2 != 2 || $4 != 3) serror("use [mbp+midx++]");
			}
	|	'[' reg '+' '-' '-' reg ']'
			{	opnd.kind = K_IDX; opnd.field = 2;
				if ($2 != 2 || $6 != 3) serror("use [mbp+--midx]");
			}
	|	reg
			{	opnd.kind = K_REG; opnd.field = (short)$1;}
	;
