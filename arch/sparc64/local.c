/*
 * Copyright (c) 2008 David Crawshaw <david@zentus.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "pass1.h"

NODE *
clocal(NODE *p)
{
	struct symtab *sp;
	int op;
	NODE *r, *l;

	op = p->n_op;
	sp = p->n_sp;
	l  = p->n_left;
	r  = p->n_right;

#ifdef PCC_DEBUG
	if (xdebug) {
		printf("clocal in: %p, %s\n", p, copst(op));
		fwalk(p, eprint, 0);
	}
#endif

	switch (op) {
	case NAME:
		if (sp->sclass == PARAM || sp->sclass == AUTO) {
			/*
			 * Use a fake structure reference to
			 * write out frame pointer offsets.
			 */
			l = block(REG, NIL, NIL, PTR+STRTY, 0, 0);
			l->n_lval = 0;
			l->n_rval = FP;
			r = p;
			p = stref(block(STREF, l, r, 0, 0, 0));
		}
		break;
	case PCONV:
		if (p->n_type > BTMASK && l->n_type > BTMASK) {
			/* Remove unnecessary pointer conversions. */
			l->n_type = p->n_type;
			l->n_qual = p->n_qual;
			l->n_df = p->n_df;
			l->n_sue = p->n_sue;
			nfree(p);
			p = l;
		}
		break;

	case FORCE:
		/* Put attached value into the return register. */
		p->n_op = ASSIGN;
		p->n_right = p->n_left;
		p->n_left = block(REG, NIL, NIL, p->n_type, 0, MKSUE(INT));
		p->n_left->n_rval = I0; /* XXX adjust for float/double */
		break;
	}

#ifdef PCC_DEBUG
	if (xdebug) {
		printf("clocal out: %p, %s\n", p, copst(op));
		fwalk(p, eprint, 0);
	}
#endif

	return p;
}

void
myp2tree(NODE *p)
{
}

int
andable(NODE *p)
{
	return 1;
}

void
cendarg()
{
	autooff = AUTOINIT;
}

int
cisreg(TWORD t)
{
	/* SPARCv9 registers are all 64-bits wide. */
	return 1;
}

NODE *
offcon(OFFSZ off, TWORD t, union dimfun *d, struct suedef *sue)
{
	return bcon(off/SZCHAR);
}

void
spalloc(NODE *t, NODE *p, OFFSZ off)
{
}

void
inwstring(struct symtab *sp)
{
}

void
instring(struct symtab *sp)
{
}

void
zbits(OFFSZ off, int fsz)
{
}

void
infld(CONSZ off, int fsz, CONSZ val)
{
}

void
ninval(CONSZ off, int fsz, NODE *p)
{
}

char *
exname(char *p)
{
	return p ? p : "";
}

TWORD
ctype(TWORD type)
{
	return type;
}

void
calldec(NODE *p, NODE *q) 
{
}

void
extdec(struct symtab *q)
{
}

void
defzero(struct symtab *sp)
{
}

int
mypragma(char **ary)
{
	return 0;
}

void
fixdef(struct symtab *sp)
{
}