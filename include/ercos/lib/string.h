/*
    Copyright 2004-2005 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT on ERC-32.
*/

/*
 * File:    $Source$
 *
 * $Id: string.h 1281 2008-06-17 18:24:57Z cesar $
 *
 * $Revision: 1281 $
 * $Date: 2008-06-17 20:24:57 +0200 (mar, 17 jun 2008) $
 * Last CVS commit by $Author: cesar $
 */

#ifndef _STRING_H_
#define _STRING_H_

#include <public/glue.h>

void *memset(void *p,int c,unsigned int n);
void * memcpy (void * dest, const void * src, size_t n);
int memcmp(const void *s1,const void *s2,unsigned int n);
char *strcpy(char *dest,const char *src);
char *strncpy(char *dest,const char *src, unsigned int n);
int strlen(const char *s);
int strnlen(const char *s, unsigned int n);

#endif
