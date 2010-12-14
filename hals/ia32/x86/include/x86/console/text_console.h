/*----------------------------------------------------------------------------
 *-------------------------      M a R T E   O S      ------------------------
 *----------------------------------------------------------------------------
 *                                                             V1.9 2009-03-19
 *
 *                    'T e x t   C o n s o l e   D r i v e r'
 *
 *                                      H
 *
 * File 'text_console.h'                                              by MAR.
 *
 * Driver for the PC text console.
 *
 * Simple console output mechanism that sends text straight to
 * CGA/EGA/VGA video memory.  It has the nice property of being
 * functional right from the start, so it can be used to debug things
 * that happen very early before any devices are initialized.
 *
 * ----------------------------------------------------------------------
 *  Copyright (C) 2000-2008, Universidad de Cantabria, SPAIN
 *
 *  MaRTE OS web page: http://marte.unican.es
 *  Contact Addresses: Mario Aldea Rivas          aldeam@unican.es
 *                     Michael Gonzalez Harbour      mgh@unican.es
 *
 * MaRTE OS  is free software; you can  redistribute it and/or  modify it
 * under the terms of the GNU General Public License  as published by the
 * Free Software Foundation;  either  version 2, or (at  your option) any
 * later version.
 *
 * MaRTE OS  is distributed  in the  hope  that  it will be   useful, but
 * WITHOUT  ANY  WARRANTY;     without  even the   implied   warranty  of
 * MERCHANTABILITY  or  FITNESS FOR A  PARTICULAR PURPOSE.    See the GNU
 * General Public License for more details.
 *
 * You should have received  a  copy of  the  GNU General Public  License
 * distributed with MaRTE  OS;  see file COPYING.   If not,  write to the
 * Free Software  Foundation,  59 Temple Place  -  Suite 330,  Boston, MA
 * 02111-1307, USA.
 *
 * As a  special exception, if you  link this  unit  with other  files to
 * produce an   executable,   this unit  does  not  by  itself cause  the
 * resulting executable to be covered by the  GNU General Public License.
 * This exception does  not however invalidate  any other reasons why the
 * executable file might be covered by the GNU Public License.
 *
 *---------------------------------------------------------------------------*/
#ifndef __MARTE_OS_TEXT_CONSOLE_DRIVER__
#define __MARTE_OS_TEXT_CONSOLE_DRIVER__  1

/*------------------*
 *--  Ioctl Data  --*
 *------------------*/
//  Any change here should be also reflected in
//  'text_consoleioctl.ads'
typedef struct {
  unsigned int pos;
  unsigned char text_attr;
  char character;
} text_console_ioctl_data_t;

/*---------------------*
 *--  Ioctl commads  --*
 *---------------------*/
#define TEXT_CONSOLE_SET_CURSOR_POS           200
#define TEXT_CONSOLE_ACTIVATE_SCROLL_MODE     201
#define TEXT_CONSOLE_DEACTIVATE_SCROLL_MODE   202
#define TEXT_CONSOLE_SET_TEXT_ATTR            203
#define TEXT_CONSOLE_GET_CHAR_IN_SCREEN_POS   204
#define TEXT_CONSOLE_GET_ATTR_OF_SCREEN_POS   205
#define TEXT_CONSOLE_CLRSCR                   206
#define TEXT_CONSOLE_GET_CURSOR_POS           207

/*------------------------------------------------------------*
 *--  Text Console Functions (defined in 'text_console.c')  --*
 *------------------------------------------------------------*/

void text_console_putchar(unsigned char c);
extern int text_console_create ();

extern int text_console_basic_create ();

#endif  // __MARTE_OS_TEXT_CONSOLE_DRIVER__
