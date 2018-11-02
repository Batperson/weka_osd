/*
 * memory.h
 *
 *  Created on: 26/09/2018
 *   Memory management routines
 */

#ifndef MEMORY_H_
#define MEMORY_H_

void zerobuf(void*, size_t);

// until I can roll my own in assembler, use memset

#define mset memset



#endif /* MEMORY_H_ */
