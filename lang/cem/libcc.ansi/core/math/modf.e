#
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
#include <ack/config.h>

 mes 2,_EM_WSIZE,_EM_PSIZE

#if ACKCONF_WANT_FLOAT
 exp $modf
 pro $modf,0
#if _EM_WSIZE==2 && _EM_PSIZE==2 && _EM_DSIZE==8
 mes 9,10,8
#endif
 lal 0
 loi _EM_DSIZE
 loc 1
 loc _EM_WSIZE
 loc _EM_DSIZE
 cif
 fif _EM_DSIZE
 lal _EM_DSIZE
 loi _EM_PSIZE
 sti _EM_DSIZE
 ret _EM_DSIZE
 end
#endif
