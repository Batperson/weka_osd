/*
 * misc.h
 * Utility defines
 */

#ifndef MISC_H_
#define MISC_H_

//#define IN_CCM  __attribute__((section(".ccm")))
#define IN_CCM
#define ALIGNED(n) 			__attribute__((aligned(n)))
#define INTERRUPT 			__attribute__((interrupt("IRQ")))
#define ALWAYS_INLINE 		inline __attribute__((always_inline))

#endif /* MISC_H_ */
