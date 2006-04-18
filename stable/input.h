

/*
 * $Id: input.h,v 1.1.1.1 1998/02/23 15:22:27 lindig Exp $
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


#ifndef __INPUT_H__
#define __INPUT_H__

typedef struct {
	List *objlist;		/* list of InputObj */
} Input ;

typedef struct {
	char *objstr ;		/* obj name */
	List *atrlist;		/* all adjacent atributes */
}  InputObj;

/* input.c */
extern Input *InputNew _ANSI_ARGS_((void));
extern InputObj *InputObjNew _ANSI_ARGS_((char *name, List *atrlist));
extern VOID InputAddObj _ANSI_ARGS_((Input *input, InputObj *obj));
extern VOID InputInit _ANSI_ARGS_((Input *input));
extern VOID InputDelete _ANSI_ARGS_((Input *input));
extern VOID InputToContext _ANSI_ARGS_((Input *input, Context *context));

#endif /* ! __INPUT_H__ */
