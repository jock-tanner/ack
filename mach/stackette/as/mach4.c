#define RCSID4 "$Id$"

/*
 * Stackette parsing rules
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
	;
