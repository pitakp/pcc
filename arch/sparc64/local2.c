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
#include "pass2.h"

char *
rnames[] = {
	/* "\%g0", always zero, removed due to 31-element class limit */
	        "\%g1", "\%g2", "\%g3", "\%g4", "\%g5", "\%g6", "\%g7",
	"\%o0", "\%o1", "\%o2", "\%o3", "\%o4", "\%o5", "\%o6", "\%o7",
	"\%l0", "\%l1", "\%l2", "\%l3", "\%l4", "\%l5", "\%l6", "\%l7",
	"\%i0", "\%i1", "\%i2", "\%i3", "\%i4", "\%i5", "\%i6", "\%i7",

	"\%sp", "\%fp+2047", /* XXX stack bias magic number */

	"\%f0",  "\%f1",  "\%f2",  "\%f3",  "\%f4",  "\%f5",  "\%f6",  "\%f7",
	"\%f8",  "\%f9",  "\%f10", "\%f11", "\%f12", "\%f13", "\%f14", "\%f15",
	"\%f16", "\%f17", "\%f18", "\%f19", "\%f20", "\%f21", "\%f22", "\%f23",
	"\%f24", "\%f25", "\%f26", "\%f27", "\%f28", "\%f29", "\%f30"
	/*, "\%f31" XXX removed due to 31-element class limit */
};

void
deflab(int label)
{
	printf(LABFMT ":\n", label);
}

void
prologue(struct interpass_prolog *ipp)
{
	int i, j, stack;

	/* XXX: stack bias magic number */
	for (stack=p2maxautooff+192, i=ipp->ipp_regs, j=0; i; i >>= 1, j++) {
		/* XXX: we don't always need this much stack */
		if (i & 1)
			stack += 8;
	}

	/* TODO printf("\t.proc %d\n"); */
	printf("\t.global %s\n", ipp->ipp_name);
	printf("\t.align 4\n");
	printf("%s:\n", ipp->ipp_name);
	printf("\tsave %%sp,-%d,%%sp\n", stack);
}

void
eoftn(struct interpass_prolog *ipp)
{
	printf("\tret\n");
	printf("\trestore\n");
	printf("\t.type %s,#function\n", ipp->ipp_name);
	printf("\t.size %s,(.-%s)\n", ipp->ipp_name, ipp->ipp_name);
}

void
hopcode(int f, int o)
{
	char *str;

	switch (o) {
		case EQ:        str = "beq"; break;
		case NE:        str = "bne"; break;
		case ULE:
		case LE:        str = "ble"; break;
		case ULT:
		case LT:        str = "bl";  break;
		case UGE:
		case GE:        str = "bge"; break;
		case UGT:
		case GT:        str = "bg";  break;
		case PLUS:      str = "add"; break;
		case MINUS:     str = "sub"; break;
		case AND:       str = "and"; break;
		case OR:        str = "or";  break;
		case ER:        str = "xor"; break;
		default:
			comperr("unknown hopcode: %d (with %c)", o, f);
			return;
	}

	printf("%s%c", str, f);
}

int
tlen(NODE *p)
{
	switch (p->n_type) {
		case CHAR:
		case UCHAR:
			return 1;
		case SHORT:
		case USHORT:
			return (SZSHORT / SZCHAR);
		case DOUBLE:
			return (SZDOUBLE / SZCHAR);
		case INT:
		case UNSIGNED:
		case LONG:
		case ULONG:
			return (SZINT / SZCHAR);
		case LONGLONG:
		case ULONGLONG:
			return SZLONGLONG / SZCHAR;
		default:
			if (!ISPTR(p->n_type))
				comperr("tlen type unknown: %d");
			return SZPOINT(p->n_type) / SZCHAR;
	}
}

void
zzzcode(NODE * p, int c)
{
	printf("XXX zzzcode called\n"); /* XXX */
}

int
rewfld(NODE * p)
{
	return (1);
}

int
fldexpand(NODE *p, int cookie, char **cp)
{
	printf("XXX fldexpand called\n"); /* XXX */
	return 1;
}

int
flshape(NODE * p)
{
	return SRREG;
}

int
shtemp(NODE * p)
{
	return 0;
}


void
adrcon(CONSZ val)
{
}

void
conput(FILE * fp, NODE * p)
{
	if (p->n_op != ICON) {
		comperr("conput got bad op");
		return;
	}

	if (p->n_name[0] != '\0') {
		fprintf(fp, "%s", p->n_name);
		if (p->n_lval)
			fprintf(fp, "+%d", (int)p->n_lval);
	} else
		fprintf(fp, CONFMT, p->n_lval & 0xffffffff);
}

void
insput(NODE * p)
{
	comperr("insput");
}

void
upput(NODE *p, int size)
{
	comperr("upput");
}

void
adrput(FILE * io, NODE * p)
{
	if (p->n_op == FLD) {
		printf("adrput a FLD\n");
		p = p->n_left;
	}

	switch (p->n_op) {
	case NAME:
		if (p->n_name[0] != '\0')
			fputs(p->n_name, io);
		if (p->n_lval != 0)
			fprintf(io, "+" CONFMT, p->n_lval);
		return;
	case OREG:
		fprintf(io, "%s", rnames[p->n_rval]);
		if (p->n_lval)
			fprintf(io, "%d", (int) p->n_lval);
		return;
	case ICON:
		/* addressable value of the constant */
		conput(io, p);
		return;
	case MOVE:
	case REG:
		fputs(rnames[p->n_rval], io);
		return;

	default:
		comperr("illegal address, op %d, node %p", p->n_op, p);
		return;
	}
}

void
cbgen(int o, int lab)
{
}

void
myreader(struct interpass * ipole)
{
}

void
mycanon(NODE * p)
{
}

void
myoptim(struct interpass * ipole)
{
}

void
rmove(int s, int d, TWORD t)
{
	printf("\tmov %s,%s\n", rnames[d], rnames[s]);
}

int
gclass(TWORD t)
{
	return (t == FLOAT || t == DOUBLE || t == LDOUBLE) ? CLASSC : CLASSA;
}

void
lastcall(NODE *p)
{
}

int
special(NODE *p, int shape)
{
	return SRNOPE;
}

void mflags(char *str)
{
}

int
COLORMAP(int c, int *r)
{
	int num=0;

	switch (c) {
		case CLASSA:
			num += r[CLASSA];
			return num < 32;
		case CLASSB:
			return 0;
		case CLASSC:
			num += r[CLASSC];
			return num < 32;
		default:
			comperr("COLORMAP: unknown class");
			return 0;
	}
}