
/*
 * $Id: print.c,v 1.3 1998/02/25 16:22:07 lindig Exp $
 *
 * CONCEPTS
 * Copyright (C) 1994 Technical University of Braunschweig, Germany
 * Written by Christian Lindig (lindig@ips.cs.tu-bs.de)
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Handle all output aspects.
 */

#include "config.h"
#include <stdio.h>
#if defined(STDC_HEADERS) || defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#endif

#if STDC_HEADERS || defined(HAVE_STRING_H)
#  include <string.h>
#else
#  ifndef HAVE_MEMCPY
#    define memcpy(d, s, n) bcopy ((s), (d), (n))
#    define memmove(d, s, n) bcopy ((s), (d), (n))
#  endif
#endif

#include "defines.h"
#include "panic.h"
#include "set.h"
#include "list.h"
#include "hash.h"
#include "relation.h"
#include "concept.h"
#include "context.h"
#include "arrow.h"
#include "print.h"

/*
 * requires: ArrowDown ArrowUp ConceptNumber ContextRelated
 * ContextToRel RelDelete SetFirst SetNext fprintf fputc free sprintf
 * strlen */

/*
 * defines
 */

#define LINEBREAK 65

/*
 * private prototypes 
 */

static VOID PrintSet _ANSI_ARGS_((FILE *f, Set *set, char **names, 
				  char sep, int *col));
static VOID PrintConcept _ANSI_ARGS_((FILE *f, char *fmt, int *col, 
				      ContextLattice *lattice, int id));
static VOID PrintFormatted _ANSI_ARGS_((FILE *f, char *str, 
					int len, int *col));
static VOID PrintRelatedAttributes _ANSI_ARGS_((FILE *f, 
						 char *fmt, 
						 Relation *rel, 
						 char **obj, char **atr, 
						 int *col, int id));

/*
 * PrintStrOrder
 */

static int
PrintStrOder (s1,s2)
	char **s1;
	char **s2;

{
	return strcmp(*s1,*s2);
}

/*
 * printSet 
 * print out a set. Members are seperated by a <sep>
 * charakter; if *col !=0 line braking is enabled and *col holds the
 * current output colmun. Sets contain integers. Each integer may be
 * associated a name through an array <names>. If no such array is
 * supplied the intergers will be printed instaed */

static VOID
PrintSet (f,set,names,sep,col) 

	FILE *f;		/* output goes here */
	Set *set;		/* set to print */
	char **names;		/* array of set member names*/
	char sep;		/* seperator between elements */
	int *col;		/* actual output column */
	

{
	int elem;		/* set element */
	char buf[20];		/* for integer as string */
	char **sorted_names;    
	char **sorted;
	int size;

	size = SetSize(set);
	sorted_names = (char**) malloc (sizeof(char*) * size);
	if (!sorted_names)
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	sorted = sorted_names;
	
	if (names) {
		elem = SetFirst(set);
		while (elem != -1) {
			*sorted++ = names[elem];
			elem = SetNext (set,elem);
		}
		qsort ((VOID*)sorted_names, size, sizeof(char*),PrintStrOder);
		for (elem=0;elem<size;elem++) {
			PrintFormatted (f,sorted_names[elem],
					strlen(sorted_names[elem]),col);
			if (elem != size-1)
				PrintFormatted (f,&sep,1,col);
		}
		free ((char*) sorted_names);
	} else { /* just print out the numbers */
		elem = SetFirst (set);
		while (elem != -1) {
			sprintf (buf,"%i",elem);
			PrintFormatted (f,buf,strlen(buf),col);
			elem = SetNext (set,elem);
			if (elem != -1) {
				PrintFormatted (f,&sep,1,col);
			}
		}
	}
}

/*
 * PrintFormatted 
 * prints out a string and inserts newlines for
 * formatting. Maintains the current output column. Formatting does
 * not take place if *col == 0 and *col will remain untouched in this
 * case.  */

static VOID
PrintFormatted (f,str,len,col)
	FILE *f;		/* output stream */
	char *str;		/* string to be printed (not \0 terminated!)*/
	int len;		/* length of string */
	int *col;		/* output column*/

{
	if (len == 0)
		return;		/* nothing to do */
	
	if (!*col) {
		/* no formatting */
		while (len--)
			fputc (*str++,f);
	} else {
		if (*col > LINEBREAK && *str != '\n') {
			fputc ('\n',f);
			*col = 1;
		}
		while (len--) {
			switch (*str) {
			case '\n':
				*col = 1;
				break;
			case '\t':
				*col += 8 - (*col % 8);
				break;
			default:
				(*col)++ ;
			}
			fputc (*str++,f);
		}
	}
}

/*
 * printLattice 
 *
 * prints out all concepts from a concept lattice according to a user
 * supplied format string.
 */

VOID
PrintLattice (f,fmt,lattice)
	FILE *f;			/* output goes here */
	char *fmt;			/* format string */
	ContextLattice *lattice;	/* concept lattice */

{
	int c;			/* concept index */
	int col;		/* output column */

	col = 1 ;		/* line breaking enabled (default)*/
	
	/* print all concepts */

	for (c=0;c<lattice->conc;c++) {
		PrintConcept (f,fmt,&col,lattice,c);
	}
}

/*
 * PrintCheck
 *
 * checks a format string for minimal corectness. Returns an  error
 * message on error and NULL otherwise */

const char *
PrintCheck (fmt)
	char *fmt;		/* format string*/

{
	char *p;		/* actual fmt character */
	char *error;

	error = (char *) NULL;	/* no error */
	p = fmt;		/* first character */
	while ( *p && !error ) {
		if (*p != '%') {
			p++;
			continue ;
		}
		switch (*++p) {
		case '\0':
			error = "\"%\" must be follwed by format flag";
			break;
		case 'o': 
		case 'O':
		case 'a':
		case 'A':
		case '<':
		case '>': 
		case '+':
		case '-':
		case 'i':
		case 'n':
		case 't':
		case '%':
			break ;	/* ok */
		case ':':
			if (*++p == '\0') {
				error = "\"%:\" must be followed by character";
			}
			break;

		default:
			error = "unknown format flag";
		}
		p++ ;		/* next char */
	}
	return error ;
}


/*
 * PrintConcept
 *
 * print a concept according to a format string - inspired by printf. 
 * Format characters:
 *
 * %:x  sets seperator of list elements to x
 * %+   line breaking on
 * %-   line breaking off
 * %i	integer index of the concept
 * %O   %:x seperated list of objects 
 * %A   %:x seperated list of attributs
 * %o   %:x seperated list of objects introduced by the current concept
 * %a   %:x seperated list of attribs introduced by the current concept
 * %<   %:x separated list of all subconcepts
 * %>   %:x seperated list of all superconcepts
 * %n	Newline
 * %t	Tab
 *
 * if col != 0 col holds the current output column and line breaking
 * is enabled, and disable otherwise.  */

static VOID
PrintConcept (f,fmt,col, lattice, id)
	FILE		*f;		/* output goes here */
	char		*fmt;		/* format string */
	ContextLattice	*lattice;	/* lattice of all concepts */
	int		*col;		/* current output column */
	int		id;		/* concept to be printed */
	

{
	char *p;		/* actual format char */
	Concept *c;		/* concept to be printed */
	char sep;		/* seperator */
	Set *set;		/* general purpose set */
	char buf[20];		/* buffer for sprintf */

	sep = ' ';		/* default: space */
	c = lattice->concepts[id];

	for (p = fmt; *p; p++) {
		if (*p != '%') {
			PrintFormatted(f,p,1,col);
			continue;
		}
		switch (*++p) {
		case 'i':
			sprintf (buf,"%03i",id);
			PrintFormatted(f,buf,strlen(buf),col);
			break;
		case 'O':
			set = &c->objects; /* all objects */
			PrintSet (f,set,lattice->objects,sep,col);
			break;
		case 'A':
			set = &c->attributes; /* all attributes */
			PrintSet (f,set,lattice->attributes,sep,col);
			break;
		case 'o':
			set = &c->obj; /* new objects */
			PrintSet (f,set,lattice->objects,sep,col);
			break;
		case 'a':
			set = &c->atr; /* all attributes */
			PrintSet (f,set,lattice->attributes,sep,col);
			break;
		case '+':
			*col = 1; /* enable line breaking */
			break;
		case '-':
			*col = 0; /* disable line breaking */
			break;
		case ':':
			sep = *(++p);
			break;
		case '<':
			set = &c->closure ; /* all subconcepts */
			PrintSet (f,set,(char**)NULL,sep,col);
			break;
		case '>':
			set = &c->superclos ; /* all superconcepts */
			PrintSet (f,set,(char**)NULL,sep,col);
			break;
		case 'n':
			PrintFormatted (f,"\n",1,col);
			break;
		case 't':
			PrintFormatted (f,"\t",1,col);
			break;
		default:
			PrintFormatted (f,p,1,col);
		}
	}
}
			

/*
 * PrintGraph
 *
 * write out a concept lattice suitable as graphplace(1) input
 */

VOID 
PrintGraph (lattice,print_names,out)

	ContextLattice *lattice; 	/* eveything we need to know */
	int print_names;
	FILE *out;			/* output stream */

{
	Set *atrsetmin ;	/* recently introd. attrs */
	Set *objsetmin;		/* recently introd. objs */
	Set *subcset;	 	/* subconcepts of a given set */
	int sub;		/* subconcept index */
	Concept *concept;	/* general concept */
	int i;			/* counter */
	int obj,atr;		/* object and attribute in set */
	
	/* some statistics */

	fprintf (out,"%%!PS\n");
	fprintf (out,"%%%% number of objects: %i\n",lattice->obj);
	fprintf (out,"%%%% number of attributes: %i\n",lattice->atr);
	fprintf (out,"%%%% number of concepts: %i\n",lattice->conc);
	fprintf (out,"\n");

	/* print all concepts - each nodes is annotated by the 
	   objects (below) and attributes (above) it introduces */
		 
	/* output: (obj1 obj2 ..) () (atr1 atr2..) nummer node */

	for (i=0;i<lattice->conc;i++) {
		concept = lattice->concepts[i];
		objsetmin = &concept->obj; /* NEW objects */
		atrsetmin = &concept->atr; /* ALL attributes */

		fprintf (out,"(");
		if (print_names) {
			obj = SetFirst (objsetmin);
			while (obj != -1) {
				fprintf (out,"%s",lattice->objects[obj]);
				obj = SetNext (objsetmin,obj);
				if (obj != -1)
					fprintf (out," ");
			}
		} else {
			fprintf (out,"%i",i);			    
		}
		fprintf (out,") ");

		/* write no string right onto the node */

		fprintf (out," () ");

		if (print_names) {
			fprintf (out,"(");
			atr = SetFirst (atrsetmin);
			while (atr != -1) {
				fprintf (out,"%s",lattice->attributes[atr]);
				atr = SetNext (atrsetmin,atr);
				if (atr != -1)
					fprintf (out," ");
			}
			fprintf (out,") ");
		} else {
			fprintf (out,"() ");
		}
		fprintf (out,"%i node\n",i);
	}

	fprintf (out,"\n");

	/* print out all edges */

	for (i=0;i<lattice->conc;i++) {
		concept = lattice->concepts[i];
		subcset = &concept->subconcepts;

		sub = SetFirst (subcset);
		while (sub != -1) {
			fprintf (out,"%i %i edge\n",i,sub);
			sub = SetNext (subcset,sub);
		}
	}
}

/*
 * PrintStat
 *
 * print out a small statictic about the current context - especially
 * the number of concepts */

VOID
PrintStat (context,out)

	Context *context ;
	FILE *out;

{

	Relation rel;
	char **objstr,**atrstr;	/* all names of obj and atr */
	int obj,atr;
	int count ;
	int i;

	/* create Relation from Context */

	ContextToRel(context,&rel,&objstr,&atrstr,&obj,&atr);
	
	/* calculate concepts */

	count = ConceptNumber(&rel);

	/* free memory */
	
	RelDelete (&rel);

	for (i=0;i<obj;i++) {
		free(objstr[i]);
	}
	free ((char*)objstr);

	for (i=0;i<atr;i++) {
		free(atrstr[i]);
	}
	free ((char*)atrstr);

	/* print out results */

	fprintf(out,"number of objects:    %i\n",obj);
	fprintf(out,"number of attributes: %i\n",atr);
	fprintf(out,"number of concepts:   %i\n",count);

}

/*
 * PrintRelatedAttributes 
 *
 * print for a object of a relation all
 * associated attributes according to a format string:
 * Format characters:
 *
 * %:x  sets seperator of list elements to x
 * %+   line breaking on
 * %-   line breaking off
 * %i	integer index of the object the attr. are related to
 * %O   object
 * %A   %:x seperated list of attributes
 * %o   object
 * %a   %:x seperated list of attributes
 * %n	Newline
 * %t	Tab
 */

static VOID
PrintRelatedAttributes(f,fmt,rel,obj,atr,col,id)

	FILE *f;
	char *fmt;
	Relation *rel;
	char **obj;
	char **atr;
	int *col;
	int id;
{
	char *p;		/* actual format char */
	char sep;		/* seperator */
	Set *set;		/* general purpose set */
	char buf[20];		/* buffer for sprintf */
	sep = ' ';		/* default: space */

	for (p = fmt; *p; p++) {
		if (*p != '%') {
			PrintFormatted(f,p,1,col);
			continue;
		}
		switch (*++p) {
		case 'i':
			sprintf (buf,"%03i",id);
			PrintFormatted(f,buf,strlen(buf),col);
			break;
		case 'O':
		case 'o':
			PrintFormatted (f,obj[id],strlen(obj[id]),col);
			break;
		case 'A':
		case 'a':
			set = RelObj(rel,id); /* all attributes */
			PrintSet (f,set,atr,sep,col);
			break;
		case '+':
			*col = 1; /* enable line breaking */
			break;
		case '-':
			*col = 0; /* disable line breaking */
			break;
		case ':':
			sep = *(++p);
			break;
		case 'n':
			PrintFormatted (f,"\n",1,col);
			break;
		case 't':
			PrintFormatted (f,"\t",1,col);
			break;
		case '>':
		case '<':
		default:
			PrintFormatted (f,p,1,col);
		}
	}
}

/*
 * PrintRelation
 *
 * print a relation according to a format string. The names of the
 * objects are supplied by the array obj, the names of attributes by
 * atr.  */

VOID PrintRelation (f,fmt,rel,obj,atr) 

	FILE *f;		/* output channel */
	char *fmt;		/* format string  */
	Relation *rel;		/* relation to print */
	char **obj;		/* array of assciated strings for obj */
	char **atr;		/* ditto for atr */
{
	int o;
	int col;

	col = 1;		/* line breaking enabled */
	for (o=0;o<rel->maxobj;o++) {
		PrintRelatedAttributes (f,fmt,rel,obj,atr,&col,o);
	}
}
