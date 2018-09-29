/*
 * memory.h
 *
 *  Created on: 26/09/2018
 *   Memory management routines
 */

#ifndef MEMORY_H_
#define MEMORY_H_

ALWAYS_INLINE void wordset(void* pdest, u32 val, size_t n )
{
	u32* ps = (u32*)pdest;
	u32* pe = ps + n;
	while(ps++ < pe)
		*ps = val;
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
