/*
 * misc.h
 * Utility defines
 */

#ifndef MISC_H_
#define MISC_H_

// use #ifdef __CDT_PARSER__ if necessary to hide constructs from the indexer

#define IN_CCM  			/* __attribute__((section(".ccmram"))) */
#define IN_RAM  			__attribute__((section(".data")))
#define ALIGNED(n) 			__attribute__((aligned(n)))
#define INTERRUPT 			__attribute__((interrupt("IRQ")))
#define ALWAYS_INLINE 		inline __attribute__((always_inline))

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#endif /* MISC_H_ */
