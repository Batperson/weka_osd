/*
 * video.h
 * Video timing and DMA
 */

#ifndef VIDEO_H_
#define VIDEO_H_

void initVideo();

ALWAYS_INLINE u8 currentScanLine() { return TIM2->CNT; }

#endif /* VIDEO_H_ */
