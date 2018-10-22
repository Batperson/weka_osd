/***************************************************************************
 * STM32 VGA demo
 * Copyright (C) 2012 Artekit Italy
 * http://www.artekit.eu
 * Written by Ruben H. Meleca

### font5x7.c

#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

***************************************************************************/
#include "stm32f4xx.h"
#include "misc.h"
#include "graphics.h"

u8 systemFontData[] IN_RAM =
{
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,		// 0x20 ' '
	0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x00, 		// 0x20 '!'
	0x00, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 '"'
	0x00, 0x00, 0x50, 0xF8, 0x50, 0xF8, 0x50, 0x00, 0x00, 		// 0x20 '#'
	0x00, 0x20, 0x78, 0xA0, 0x70, 0x28, 0xF0, 0x20, 0x00, 		// 0x20 '$'
	0x00, 0xC8, 0xD0, 0x20, 0x20, 0x20, 0x58, 0x98, 0x00, 		// 0x20 '%'
	0x00, 0x20, 0x50, 0x20, 0x40, 0xA8, 0x98, 0x70, 0x00, 		// 0x20 '&'
	0x00, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 '''
	0x00, 0x20, 0x40, 0x80, 0x80, 0x80, 0x40, 0x20, 0x00, 		// 0x20 '('
	0x00, 0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20, 0x00, 		// 0x20 ')'
	0x00, 0x00, 0xa8, 0x70, 0xf8, 0x70, 0xa8, 0x00, 0x00, 		// 0x20 '*'
	0x00, 0x00, 0x20, 0x20, 0xf8, 0x20, 0x20, 0x00, 0x00, 		// 0x20 '+'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x40, 0x00, 		// 0x20 ','
	0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 		// 0x20 '-'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 		// 0x20 '.'
	0x00, 0x10, 0x10, 0x20, 0x20, 0x20, 0x40, 0x40, 0x00, 		// 0x20 '/'
	0x00, 0x70, 0x88, 0x88, 0xA8, 0x88, 0x88, 0x70, 0x00, 		// 0x20 '0'
	0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00, 		// 0x20 '1'
	0x00, 0x70, 0x88, 0x08, 0x70, 0x80, 0x80, 0xF8, 0x00, 		// 0x20 '2'
	0x00, 0x70, 0x88, 0x08, 0x30, 0x08, 0x88, 0x70, 0x00, 		// 0x20 '3'
	0x00, 0x10, 0x30, 0x50, 0x90, 0xF8, 0x10, 0x10, 0x00, 		// 0x20 '4'
	0x00, 0xf0, 0x80, 0x80, 0xf0, 0x08, 0x88, 0x70, 0x00, 		// 0x20 '5'
	0x00, 0x30, 0x40, 0x80, 0xB0, 0xC8, 0x88, 0x70, 0x00, 		// 0x20 '6'
	0x00, 0xF8, 0x08, 0x10, 0x20, 0x40, 0x80, 0x80, 0x00, 		// 0x20 '7'
	0x00, 0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00, 		// 0x20 '8'
	0x00, 0x70, 0x88, 0x88, 0x78, 0x08, 0x88, 0x70, 0x00, 		// 0x20 '9'
	0x00, 0x00, 0x30, 0x30, 0x00, 0x30, 0x30, 0x00, 0x00, 		// 0x20 ':'
	0x00, 0x00, 0x30, 0x30, 0x00, 0x30, 0x30, 0x60, 0x00, 		// 0x20 ';'
	0x00, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x00, 		// 0x20 '<'
	0x00, 0x00, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0x00, 0x00, 		// 0x20 '='
	0x00, 0x40, 0x20, 0x10, 0x08, 0x10, 0x20, 0x40, 0x00, 		// 0x20 '>'
	0x00, 0x70, 0x88, 0x08, 0x10, 0x20, 0x00, 0x20, 0x00, 		// 0x20 '?'
	0x00, 0x70, 0x88, 0xb8, 0xa8, 0xb8, 0x80, 0x78, 0x00, 		// 0x20 '@'
	0x00, 0x70, 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00, 		// 0x20 'A'
	0x00, 0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0xF0, 0x00, 		// 0x20 'B'
	0x00, 0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00, 		// 0x20 'C'
	0x00, 0xF0, 0x88, 0x88, 0x88, 0x88, 0x88, 0xF0, 0x00, 		// 0x20 'D'
	0x00, 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xF8, 0x00, 		// 0x20 'E'
	0x00, 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0x00, 		// 0x20 'F'
	0x00, 0x70, 0x88, 0x80, 0xB8, 0x88, 0x88, 0x70, 0x00, 		// 0x20 'G'
	0x00, 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00, 		// 0x20 'H'
	0x00, 0x70, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00, 		// 0x20 'I'
	0x00, 0x08, 0x08, 0x08, 0x08, 0x88, 0x88, 0x70, 0x00, 		// 0x20 'J'
	0x00, 0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x88, 0x00, 		// 0x20 'K'
	0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF8, 0x00, 		// 0x20 'L'
	0x00, 0x88, 0xD8, 0xA8, 0x88, 0x88, 0x88, 0x88, 0x00, 		// 0x20 'M'
	0x00, 0x88, 0xC8, 0xA8, 0x98, 0x88, 0x88, 0x88, 0x00, 		// 0x20 'N'
	0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 		// 0x20 'O'
	0x00, 0xf0, 0x88, 0x88, 0xf0, 0x80, 0x80, 0x80, 0x00, 		// 0x20 'P'
	0x00, 0x70, 0x88, 0x88, 0x88, 0xa8, 0x90, 0x68, 0x00, 		// 0x20 'Q'
	0x00, 0xf0, 0x88, 0x88, 0xf0, 0xa0, 0x90, 0x88, 0x00, 		// 0x20 'R'
	0x00, 0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70, 0x00, 		// 0x20 'S'
	0x00, 0xf8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 		// 0x20 'T'
	0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 		// 0x20 'U'
	0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, 		// 0x20 'V'
	0x00, 0x88, 0x88, 0x88, 0x88, 0xa8, 0xa8, 0x70, 0x00, 		// 0x20 'W'
	0x00, 0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00, 		// 0x20 'X'
	0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x00, 		// 0x20 'Y'
	0x00, 0xf8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xf8, 0x00, 		// 0x20 'Z'
	0x00, 0x70, 0x40, 0x40, 0x40, 0x40, 0x40, 0x70, 0x00, 		// 0x20 '['
	0x00, 0x40, 0x40, 0x20, 0x20, 0x20, 0x10, 0x10, 0x00, 		// 0x20 '\'
	0x00, 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x70, 0x00, 		// 0x20 ']'
	0x00, 0x20, 0x50, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 '^'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 		// 0x20 '_'
	0x00, 0x20, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 '`'
	0x00, 0x00, 0x00, 0x70, 0x08, 0x78, 0x98, 0x68, 0x00, 		// 0x20 'a'
	0x00, 0x00, 0x80, 0x80, 0xF0, 0x88, 0x88, 0xF0, 0x00, 		// 0x20 'b'
	0x00, 0x00, 0x00, 0x78, 0x80, 0x80, 0x80, 0x78, 0x00, 		// 0x20 'c'
	0x00, 0x00, 0x08, 0x08, 0x78, 0x88, 0x88, 0x78, 0x00, 		// 0x20 'd'
	0x00, 0x00, 0x00, 0x70, 0x88, 0xF0, 0x80, 0x70, 0x00, 		// 0x20 'e'
	0x00, 0x30, 0x40, 0x40, 0x60, 0x40, 0x40, 0x40, 0x00, 		// 0x20 'f'
	0x00, 0x00, 0x00, 0x78, 0x88, 0x78, 0x08, 0x70, 0x00, 		// 0x20 'g'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'h'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'i'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'j'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'k'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'l'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'm'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'n'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'o'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'p'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'q'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'r'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 's'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 't'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'u'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'v'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'w'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'x'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'y'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 'z'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 '{'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 '|'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 '}'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// 0x20 '~'
	0x00, 0xFC, 0x84, 0x84, 0x84, 0x84, 0xFC, 0x00, 0x00, 		// 0x20 ''
};

FONT systemFont IN_RAM = { 6, 9, 1, 0, systemFontData };
