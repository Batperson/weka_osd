/*
 * system.h
 * system and OS-like utility functions
 *  Created on: 21/08/2018
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

extern volatile u32 sysTicks;
extern volatile u8 blink;

void sleep(u32 millisecs);
ALWAYS_INLINE u8 blinkOn() { return blink; }
ALWAYS_INLINE u32 millis() { return sysTicks; }

void initSystem();

#endif /* SYSTEM_H_ */
