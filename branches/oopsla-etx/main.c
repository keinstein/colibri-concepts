
/* 
 * $Id: main.c,v 1.3 1998/02/25 16:22:06 lindig Exp $
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
 */ 

#include "config.h"
#include <stdio.h>

#if defined(STDC_HEADERS) || defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>		/* for getopt() */
#endif

#include "defines.h"
#include "version.h"		/* contains char *version */
#include "panic.h"
#include "list.h"
#include "hash.h"
#include "set.h"
#include "relation.h"
#include "concept.h"
#include "context.h"
#include "arrow.h"
#include "input.h"
#include "print.h"


/*
 * extern variables
 */

extern int yydebug ;		/* debug parser if == 1 */
extern int yyparse();		/* parser */
extern FILE *yyin;		/* file the parser reads from */

/*
 * global variables
 */

Input *maininput;		/* filled by the parser - parse tree*/
FILE *out;			/* file output, input is read from yyin */

/*
 * defines
 */

#define CONCEPTS 1
#define GRAPH_WITH_NUMBERS 2
#define GRAPH_WITH_NAMES 3
#define ARROWS 4
#define SIZE 5
#define DOT 6

#define CONCEPTS_FORMAT_c "   objects[%i]:%t%o%nattributes[%i]:%t%a%n"
#define CONCEPTS_FORMAT_C "   objects[%i]:%t%O%nattributes[%i]:%t%A%n"

#define ARROWS_FORMAT "%O: %A;%n"

/*
 * private prototypes
 */

int main _ANSI_ARGS_((int argc, char **argv));
static int doit _ANSI_ARGS_((char *format, int what));

/*
 * Usage info string array
 */

static char *usage = 
	"Usage: %s [-o file] [-f format] -c | -C | -g | -G | -d | -a | -s [file]\n";
	
/*
 * main
 */

int
main (argc,argv)

	int argc;		/* number of arguments */
	char **argv;		/* argument vekctor */
{
	char *progname;		/* name of this program */
        int c;			/* for getopt() */
        extern char *optarg;	/* for getopt() */
        extern int optind;	/* for getopt() */

	/* various variables to remember command line arguments */

	char *format;		/*  format string for -c */
	char *outfile;		/* filename from -o */
	char *infile;		/* supplied filename */
	int graph;		/* flag -g */
	int Graph;		/* flag -G */
	int size;		/* flag -s */
	int arrows;		/* flag -a */
	int concepts;		/* flag -c */
        int dot;                /* flag -d */
	int errflag;		/* error flag command line parsing */

	const char *errmsg;	/* error message from format scanning */
	int code;


	/* remember program name and initialize all the flags */

	progname = argv[0];
	format = (char*) NULL ; /* no default string yet */
	outfile = (char*) NULL;	/* no files supplied */
	infile = (char*) NULL;
	graph = Graph = size = arrows = concepts = dot = 0;

	yyin = stdin;
	out = stdout;
	errflag = 0;
	
	/* get options with getopt() - there are much more clever
           getopt packages out there, especially for handling long
           option names */

	while ((c=getopt(argc,argv,"o:f:dascCgGh")) != -1) {
		switch (c) {
		case 'a':
			arrows = 1;
			format = ARROWS_FORMAT ;
			break;
		case 'g':
			graph = 1;
			break ;
		case 'G':
			Graph = 1;
			break ;
		case 'd':
			dot = 1;
			break ;
		case 's':
			size = 1;
			break ;
		case 'c':
			concepts = 1;
			format = format ? format : CONCEPTS_FORMAT_c ;
			break ;
		case 'C':
			concepts = 1;
			format = format ? format : CONCEPTS_FORMAT_C ;
			break ;
                case 'o':
			outfile = optarg ;
			break ;
		case 'f':
			format = optarg ;
			break ;
		case 'h':
			errflag++; /* not really */
			break;
		case '?':
		default:
			errflag++;
		}
	}
	if (errflag 
	    || (arrows+size+graph+concepts+Graph+dot != 1)) {
		fprintf (stderr,version);
		fprintf (stderr,usage,progname);
		return 1;
	}
	
	/* check format string if any */
	errmsg = (format ? PrintCheck (format) : (char*) NULL);
	if (errmsg) {
		fprintf (stderr,"illegal format string supplied to -f: %s\n",
			 errmsg);
		fprintf (stderr,usage,progname);
		return 1;
	}

	/* there may be at least one (input) file argument */
	
	if (optind < argc - 1) {
		fprintf (stderr,"too many input files specified\n");
		fprintf (stderr,usage,progname);
		return 1;
	}

	if (optind < argc) {
		infile = argv[optind]; /* input file name */
	}

	/* all options processed - now open input and output files
	   and start working */

	if (infile) {
		yyin = fopen (infile,"r");
		if (!yyin) {
			panic ("can't open input file %s",infile);
		}
	}
	if (outfile) {
		out = fopen (outfile,"w");
		if (!out) {
			panic ("can't open output file %s",outfile);
		}
	}

	
	if (concepts) {
		code = doit (format,CONCEPTS) ;
	} else if (graph) {
		code = doit (format,GRAPH_WITH_NUMBERS);
	} else if (dot) {
		code = doit (format,DOT);
	} else if (Graph) {
		code = doit (format,GRAPH_WITH_NAMES);
	} else if (arrows) {
		code = doit (format,ARROWS);
	} else if (size) {
		code = doit (format,SIZE);
	} else {
		code = 1;	/* just to stop gcc complaining */
		panic ("internal error in %s:%i",__FILE__,__LINE__);
	}

	fclose (yyin);
	fclose (out);
	return code;
	
}

/*
 * doit
 */

static int 
doit (format,what)
	char *format;		/* format for concepts */
	int what;		/* what to do */
{
	Context *context;
	ContextLattice *lattice;
	Relation up,down;	/* Arrow Relation */
	int fail;


	maininput = (Input*) NULL;
	fail = yyparse();
	if (fail) {
		/* parse error occured */
		if (maininput)
			InputDelete (maininput); 
		return 1;
	}

	context = ContextNew();	
	InputToContext(maininput,context);
	InputDelete (maininput); 

	switch (what) {
	case CONCEPTS:
		lattice = ContextConcepts (context);
		PrintLattice (out,format,lattice);
		ContextLatticeDelete (lattice);
		break;
	case GRAPH_WITH_NUMBERS:
		lattice = ContextConcepts (context);
		PrintGraph (lattice,0,out);
		ContextLatticeDelete (lattice);
		break;
	case GRAPH_WITH_NAMES:
		lattice = ContextConcepts (context);
		PrintGraph (lattice,1,out);
		ContextLatticeDelete (lattice);
		break;
	case DOT:
		lattice = ContextConcepts (context);
		PrintDot (lattice,out);
		ContextLatticeDelete (lattice);
		break;
        case SIZE:
		PrintStat (context,out);
		break;
	case ARROWS:
		lattice = ContextConcepts (context);
		ArrowUpDown(context,lattice,&up,&down);
		PrintRelation (out,format,&up,
			       lattice->objects,lattice->attributes );
		fprintf (out,"\n");
		PrintRelation (out,format,&down,
			       lattice->objects,lattice->attributes );
		RelDelete (&up);
		RelDelete (&down);
		ContextLatticeDelete (lattice);
		break;
	default:
		panic ("internal error - unknown flag in %s:%i",
		       __FILE__,__LINE__);
	}
	ContextDelete(context);

	return 0;
}

