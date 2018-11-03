/*
 * video.h
 * Video timing and DMA
 */

#ifndef VIDEO_H_
#define VIDEO_H_

#define FRAME_BUF_WIDTH		300
#define FRAME_BUF_HEIGHT	200

void initVideo();

ALWAYS_INLINE u8 currentScanLine() { return TIM2->CNT; }
ALWAYS_INLINE u8 currentField() { return (GPIOA->IDR & 0x04) >> 2; }


#endif /* VIDEO_H_ */
