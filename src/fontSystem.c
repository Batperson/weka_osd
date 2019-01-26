/*
 * fontTiny.c
 *
 *  Created on: 28/08/2018
*/

#include "stm32f4xx.h"
#include "misc.h"
#include "graphics.h"

static u8 fontData[] IN_RAM =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x20 ' '
	0x05, 0x40, 0x07, 0x40, 0x07, 0x40, 0x07, 0x40, 0x07, 0x40, 0x07, 0x40, 0x05, 0x40, 0x07, 0x40, 0x05, 0x40,    // 0x21 '!'
	0x15, 0x50, 0x1D, 0xD0, 0x1D, 0xD0, 0x15, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x22 '"'
	0x00, 0x00, 0x15, 0x50, 0x5D, 0xD4, 0x7F, 0xF4, 0x5D, 0xD4, 0x7F, 0xF4, 0x5D, 0xD4, 0x15, 0x50, 0x00, 0x00,    // 0x23 '#'
	0x05, 0x40, 0x07, 0x50, 0x1F, 0xF4, 0x77, 0x50, 0x1F, 0xD0, 0x17, 0x74, 0x7F, 0xD0, 0x17, 0x40, 0x05, 0x40,    // 0x24 '$'
	0x55, 0x10, 0x7D, 0x74, 0x7D, 0xD0, 0x55, 0xD0, 0x07, 0x40, 0x1D, 0x54, 0x1D, 0xF4, 0x75, 0xF4, 0x11, 0x54,    // 0x25 '%'
	0x01, 0x00, 0x07, 0x40, 0x1D, 0xD0, 0x07, 0x40, 0x1D, 0x10, 0x77, 0x74, 0x75, 0xF4, 0x1F, 0xD0, 0x05, 0x40,    // 0x26 '&'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x27 '''
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x28 '('
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x29 ')'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x2A '*'
	0x00, 0x00, 0x05, 0x40, 0x07, 0x40, 0x57, 0x54, 0x7F, 0xF4, 0x57, 0x54, 0x07, 0x40, 0x05, 0x40, 0x00, 0x00,    // 0x2B '+'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x40, 0x07, 0x40, 0x07, 0x40, 0x1D, 0x00, 0x04, 0x00,    // 0x2C ','
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x50, 0x1F, 0xD0, 0x15, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x2D '-'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x07, 0x40, 0x01, 0x00,    // 0x2E '.'
	0x00, 0x40, 0x01, 0xD0, 0x01, 0xD0, 0x07, 0x40, 0x07, 0x40, 0x07, 0x40, 0x1D, 0x00, 0x1D, 0x00, 0x04, 0x00,    // 0x2F '/'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x75, 0x74, 0x77, 0x74, 0x75, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x30 '0'
	0x15, 0x00, 0x7D, 0x00, 0x5D, 0x00, 0x1D, 0x00, 0x1D, 0x00, 0x1D, 0x00, 0x5D, 0x40, 0x7F, 0x40, 0x55, 0x40,    // 0x31 '1'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x15, 0x74, 0x1F, 0xD0, 0x75, 0x40, 0x75, 0x54, 0x7F, 0xF4, 0x55, 0x54,    // 0x32 '2'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x15, 0x74, 0x07, 0xD0, 0x15, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x33 '3'
	0x00, 0x50, 0x01, 0xD0, 0x07, 0xD0, 0x1D, 0xD0, 0x75, 0xD4, 0x7F, 0xF4, 0x55, 0xD4, 0x01, 0xD0, 0x01, 0x50,    // 0x34 '4'
	0x55, 0x50, 0x7F, 0xD0, 0x75, 0x50, 0x75, 0x40, 0x7F, 0xD0, 0x55, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x35 '5'
	0x01, 0x40, 0x07, 0xD0, 0x1D, 0x40, 0x75, 0x40, 0x7F, 0xD0, 0x75, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x36 '6'
	0x55, 0x54, 0x7F, 0xF4, 0x55, 0x74, 0x00, 0x74, 0x01, 0xD0, 0x07, 0x40, 0x1D, 0x00, 0x1D, 0x00, 0x15, 0x00,    // 0x37 '7'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x75, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x38 '8'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x75, 0x74, 0x1F, 0xF4, 0x15, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x39 '9'
	0x00, 0x00, 0x01, 0x40, 0x07, 0xD0, 0x07, 0xD0, 0x05, 0x50, 0x07, 0xD0, 0x07, 0xD0, 0x01, 0x40, 0x00, 0x00,    // 0x3A ':'
	0x00, 0x00, 0x01, 0x40, 0x07, 0xD0, 0x07, 0xD0, 0x05, 0x50, 0x07, 0xD0, 0x07, 0xD0, 0x1F, 0x40, 0x05, 0x00,    // 0x3B ';'
	0x00, 0x40, 0x01, 0xD0, 0x07, 0x40, 0x1D, 0x00, 0x74, 0x00, 0x1D, 0x00, 0x07, 0x40, 0x01, 0xD0, 0x00, 0x40,    // 0x3C '<'
	0x00, 0x00, 0x00, 0x00, 0x55, 0x54, 0x7F, 0xF4, 0x55, 0x54, 0x7F, 0xF4, 0x55, 0x54, 0x00, 0x00, 0x00, 0x00,    // 0x3D '='
	0x04, 0x00, 0x1D, 0x00, 0x07, 0x40, 0x01, 0xD0, 0x00, 0x74, 0x01, 0xD0, 0x07, 0x40, 0x1D, 0x00, 0x04, 0x00,    // 0x3E '>'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x10, 0x74, 0x01, 0xD0, 0x07, 0x40, 0x01, 0x00, 0x07, 0x40, 0x01, 0x00,    // 0x3F '?'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x40 '@'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x74, 0x74, 0x75, 0x74, 0x7F, 0xF4, 0x75, 0x74, 0x74, 0x74, 0x54, 0x54,    // 0x41 'A'
	0x55, 0x40, 0x7F, 0xD0, 0x75, 0x74, 0x75, 0x74, 0x7F, 0xD0, 0x75, 0x74, 0x75, 0x74, 0x7F, 0xD0, 0x55, 0x40,    // 0x42 'B'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x74, 0x10, 0x74, 0x00, 0x74, 0x10, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x43 'C'
	0x55, 0x40, 0x7F, 0xD0, 0x75, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x75, 0x74, 0x7F, 0xD0, 0x55, 0x40,    // 0x44 'D'
	0x55, 0x54, 0x7F, 0xF4, 0x75, 0x54, 0x75, 0x50, 0x7F, 0xD0, 0x75, 0x50, 0x75, 0x54, 0x7F, 0xF4, 0x55, 0x54,    // 0x45 'E'
	0x55, 0x54, 0x7F, 0xF4, 0x75, 0x54, 0x75, 0x50, 0x7F, 0xD0, 0x75, 0x50, 0x74, 0x00, 0x74, 0x00, 0x54, 0x00,    // 0x46 'F'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x74, 0x10, 0x74, 0x54, 0x75, 0xF4, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x47 'G'
	0x54, 0x54, 0x74, 0x74, 0x74, 0x74, 0x75, 0x74, 0x7F, 0xF4, 0x75, 0x74, 0x74, 0x74, 0x74, 0x74, 0x54, 0x54,    // 0x48 'H'
	0x15, 0x50, 0x1F, 0xD0, 0x17, 0x50, 0x07, 0x40, 0x07, 0x40, 0x07, 0x40, 0x17, 0x50, 0x1F, 0xD0, 0x15, 0x50,    // 0x49 'I'
	0x00, 0x54, 0x00, 0x74, 0x00, 0x74, 0x00, 0x74, 0x54, 0x74, 0x74, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x4A 'J'
	0x54, 0x14, 0x74, 0x74, 0x75, 0xD0, 0x77, 0x40, 0x7D, 0x00, 0x77, 0x40, 0x75, 0xD0, 0x74, 0x74, 0x54, 0x14,    // 0x4B 'K'
	0x54, 0x00, 0x74, 0x00, 0x74, 0x00, 0x74, 0x00, 0x74, 0x00, 0x74, 0x00, 0x75, 0x54, 0x7F, 0xF4, 0x55, 0x54,    // 0x4C 'L'
	0x50, 0x14, 0x74, 0x74, 0x7D, 0xF4, 0x77, 0x74, 0x75, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x54, 0x54,    // 0x4D 'M'
	0x50, 0x54, 0x74, 0x74, 0x7D, 0x74, 0x77, 0x74, 0x75, 0xF4, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x54, 0x54,    // 0x4E 'N'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x4F 'O'
	0x55, 0x40, 0x7F, 0xD0, 0x75, 0x74, 0x75, 0x74, 0x7F, 0xD0, 0x75, 0x40, 0x74, 0x00, 0x74, 0x00, 0x54, 0x00,    // 0x50 'P'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x74, 0x74, 0x75, 0x74, 0x77, 0x74, 0x75, 0xD0, 0x1F, 0x74, 0x05, 0x14,    // 0x51 'Q'
	0x55, 0x40, 0x7F, 0xD0, 0x75, 0x74, 0x75, 0x74, 0x7F, 0xD0, 0x77, 0x40, 0x75, 0xD0, 0x74, 0x74, 0x54, 0x14,    // 0x52 'R'
	0x05, 0x40, 0x1F, 0xD0, 0x75, 0x74, 0x75, 0x50, 0x1F, 0xD0, 0x15, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x53 'S'
	0x55, 0x54, 0x7F, 0xF4, 0x57, 0x54, 0x07, 0x40, 0x07, 0x40, 0x07, 0x40, 0x07, 0x40, 0x07, 0x40, 0x05, 0x40,    // 0x54 'T'
	0x54, 0x54, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x75, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x55 'U'
	0x54, 0x54, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x1D, 0xD0, 0x07, 0x40, 0x01, 0x00,    // 0x56 'V'
	0x54, 0x54, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x75, 0x74, 0x77, 0x74, 0x77, 0x74, 0x1F, 0xD0, 0x05, 0x40,    // 0x57 'W'
	0x54, 0x54, 0x74, 0x74, 0x74, 0x74, 0x1D, 0xD0, 0x07, 0x40, 0x1D, 0xD0, 0x74, 0x74, 0x74, 0x74, 0x54, 0x54,    // 0x58 'X'
	0x54, 0x54, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x1D, 0xD0, 0x07, 0x40, 0x07, 0x40, 0x07, 0x40, 0x05, 0x40,    // 0x59 'Y'
	0x55, 0x54, 0x7F, 0xF4, 0x55, 0x74, 0x01, 0xD0, 0x07, 0x40, 0x1D, 0x00, 0x75, 0x54, 0x7F, 0xF4, 0x55, 0x54,    // 0x5A 'Z'
	0x15, 0x50, 0x1F, 0xD0, 0x1D, 0x50, 0x1D, 0x00, 0x1D, 0x00, 0x1D, 0x00, 0x1D, 0x50, 0x1F, 0xD0, 0x15, 0x50,    // 0x5B '['
	0x10, 0x00, 0x74, 0x00, 0x74, 0x00, 0x1D, 0x00, 0x1D, 0x00, 0x1D, 0x00, 0x07, 0x40, 0x07, 0x40, 0x01, 0x00,    // 0x5C '\'
	0x15, 0x50, 0x1F, 0xD0, 0x15, 0xD0, 0x01, 0xD0, 0x01, 0xD0, 0x01, 0xD0, 0x15, 0xD0, 0x1F, 0xD0, 0x15, 0x50,    // 0x5D ']'
	0x01, 0x00, 0x07, 0x40, 0x1D, 0xD0, 0x74, 0x74, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x5E '^'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x54, 0x7F, 0xF4, 0x55, 0x54,    // 0x5F '_'
	0x01, 0x00, 0x07, 0x40, 0x07, 0x40, 0x01, 0xD0, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x60 '`'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x61 'a'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x62 'b'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x63 'c'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x64 'd'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x65 'e'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x66 'f'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x67 'g'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x68 'h'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x69 'i'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x6A 'j'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x6B 'k'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x6C 'l'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x6D 'm'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x6E 'n'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x6F 'o'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x70 'p'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x71 'q'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x72 'r'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x73 's'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x74 't'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x75 'u'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x76 'v'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x77 'w'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x78 'x'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x79 'y'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x7A 'z'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x7B '{'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x7C '|'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x7D '}'
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 0x7E '~'
};

FONT systemFont IN_RAM = { 7, 9, 2, 32, 6, fontData };
