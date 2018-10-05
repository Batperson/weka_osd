/*
 * memory.h
 *
 *  Created on: 26/09/2018
 *   Memory management routines
 */

#ifndef MEMORY_H_
#define MEMORY_H_

void zerobuf(void*, size_t);

ALWAYS_INLINE void byteset(u8* pd, u8 val, size_t n)
{
	for(u8* pe = pd+n; pd < pe; pd++)
		*(u8*)pd = val;
}

ALWAYS_INLINE void wordset(u32* pd, u32 val, size_t n )
{
	for(u32* pe = pd+n; pd < pe; pd++)
		*(u8*)pd = val;
}

ALWAYS_INLINE void tilecpy(void* to, const void* from, size_t sto, size_t sfrom)
{
	u32 cpy;

	do
	{
		cpy = (sto > sfrom) ? sfrom : sto;
		memcpy(to, from, cpy);
		to += cpy;
	}
	while((sto -= cpy));
}

#endif /* MEMORY_H_ */
