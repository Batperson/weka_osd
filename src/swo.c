/*
 * swo.c
 *
 */

#include "stm32f4xx.h"

int _write(int fd, char *str, int len)
{
	for(int i=0; i<len; i++)
	{
		ITM_SendChar((*str++));
		/*
		for(int j=0; j<500; j++)
		{
			if(ITM->PORT[0].u32 != 0UL)
			{
				ITM->PORT[0].u8 = (uint8_t)*str++;
				break;
			}
		}
		*/
	}

	return len;
}
