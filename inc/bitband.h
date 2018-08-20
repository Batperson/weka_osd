/*
 * bitband.h
 *
 */

#ifndef BITBAND_H_
#define BITBAND_H_

#define BITBAND_ADDR 0x20000000

#define BITBAND_ADDRESS(_item,_bitNumber)\
    (uint32_t*) ((((uint32_t)(&(_item))) & 0x60000000)\
    + 0x2000000\
    + ((((uint32_t)&(_item)) & 0x1FFFFFFF)*32)\
    + ((_bitNumber)*4))

#define BITBAND_ACCESS(_item,_bitNumber)\
    *BITBAND_ADDRESS(_item,_bitNumber)

#define INPUT_PIN(_gbio,_bitNumber)\
    BITBAND_ADDRESS(_gbio->IDR,_bitNumber)

#define OUTPUT_PIN(_gbio,_bitNumber)\
    BITBAND_ADDRESS(_gbio->ODR,_bitNumber)

#define BITBAND_PTR(_item,_bitNumber)\
    (u32*) ((((u32)((_item))) & 0x60000000)\
    + 0x2000000\
    + ((((u32)(_item)) & 0x1FFFFFFF)*32)\
    + ((_bitNumber)*4))




#endif /* BITBAND_H_ */
