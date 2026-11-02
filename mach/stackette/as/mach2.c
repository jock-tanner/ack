#define RCSID2 "$Id$"

/*
 * Stackette tokens
 */
%token <y_word> NOOPOP
%token <y_word> ABSOP
%token <y_word> GENOP
%token <y_word> STOP
%token <y_word> LEAOP
%token <y_word> BRAOP
%token <y_word> SKIPOP
%token <y_word> ADDPOP
%token <y_word> TOSNOS
%token <y_word> DDONLY
%token <y_word> REGONLY
%token <y_word> COND
%token <y_word> RETNOP
%token <y_word> FRAMEOP
%token <y_word> RETFOP
%token <y_word> BITOP
%token <y_word> UNOP
%token <y_word> GENDD
%token <y_word> STDD
%token <y_word> LEADD
%token PFP

%type <y_word> dd reg
