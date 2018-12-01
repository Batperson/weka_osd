/*
 * video.h
 * Video timing and DMA
 */

#ifndef VIDEO_H_
#define VIDEO_H_

#define FRAME_BUF_WIDTH		360
#define FRAME_BUF_HEIGHT	288
#define FRAME_BUF_SIZE   	(FRAME_BUF_WIDTH * FRAME_BUF_HEIGHT + ((128+((FRAME_BUF_WIDTH * FRAME_BUF_HEIGHT) % 128)) % 128))

void initVideo();

ALWAYS_INLINE u16 currentScanLine() { return TIM2->CNT; }
ALWAYS_INLINE u8 currentField() { return (GPIOA->IDR & 0x04) >> 2; }


#endif /* VIDEO_H_ */
