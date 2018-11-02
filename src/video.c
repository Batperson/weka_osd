/*
 * video.c
 * Low-level video routines interacting with the hardware, including scanout and line counting
 */

#include "stm32f4xx.h"
#include "misc.h"
#include "stdio.h"
#include "string.h"
#include "bitband.h"
#include "control.h"
#include "graphics.h"
#include "video.h"
#include "system.h"
#include "memory.h"

#define FRAME_BUF_SIZE   	(FRAME_BUF_WIDTH * FRAME_BUF_HEIGHT + ((128+((FRAME_BUF_WIDTH * FRAME_BUF_HEIGHT) % 128)) % 128))

static PIXEL frameBuf0[FRAME_BUF_SIZE] ALIGNED(1024);
static PIXEL frameBuf1[FRAME_BUF_SIZE] ALIGNED(1024);
volatile PPIXEL currentRenderBuf;
static volatile PPIXEL currentOutputBuf;

// This is working code for now, later will probably incorporate into PAL/NTSC-specific section.
static const u16 pixelOutputNanoseconds			= 52000;
static const u16 pixelsPerLine					= FRAME_BUF_WIDTH;	// PAL = 768*576, NTSC = 640*480
static const u16 pixelClockCycles				= 20; 				// 28 // calcAPB2TimerPeriod(pixelOutputNanoseconds / pixelsPerLine);
static const u16 activeVideoLineStart			= 50;				// PAL starts on line 23, NTSC starts on line 16?
static const u16 activeVideoLineEnd				= 50 + FRAME_BUF_HEIGHT;

u32 calcNanosecsFromAPB1TimerTicks(u16 ticks)
{
	return ((ticks + 1) * (100000000000 /  (SystemCoreClock / 2))) / 100;
}

u32 calcNanosecsFromAPB2TimerTicks(u16 ticks)
{
	return ((ticks + 1) * (100000000000 /  SystemCoreClock)) / 100;
}

u32 calcAPB1TimerPeriod(u32 timeNS)
{
	return (((SystemCoreClock / 2) / (1000000000 / timeNS)) - 1);
}

u32 calcAPB2TimerPeriod(u32 timeNS)
{
	return ((SystemCoreClock / (1000000000 / timeNS)) - 1);
}

u16 calcAPB2TimerPeriodFromHz(u32 frequency)
{
	return (SystemCoreClock / frequency) - 1;
}

u16 calcAPB1TimerPeriodFromHz(u32 frequency)
{
	return (((SystemCoreClock / 2) / frequency) - 1);
}

void initRCC()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
}

void initSyncPort()
{
	GPIO_InitTypeDef gpio;

	/* Configure PA0 for TIM8 ETR (HSYNC to start pixel clock), datasheet page 62 */
	GPIO_StructInit(&gpio);

	gpio.GPIO_Pin 		= GPIO_Pin_0;
	gpio.GPIO_Mode 		= GPIO_Mode_AF;
	gpio.GPIO_Speed 	= GPIO_Speed_50MHz;
	gpio.GPIO_OType 	= GPIO_OType_PP;
	gpio.GPIO_PuPd 		= GPIO_PuPd_NOPULL;

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM8);
	GPIO_Init(GPIOA, &gpio);

	/* Configure PA1 for TIM2 CH2 (This will be VSYNC, to reset the timer) */
	/* Configure PA5 for TIM2 ETR (External clock, this will be HSYNC to count syncs) */
	GPIO_StructInit(&gpio);

	gpio.GPIO_Pin 		= GPIO_Pin_1 | GPIO_Pin_5;
	gpio.GPIO_Mode 		= GPIO_Mode_AF;
	gpio.GPIO_Speed 	= GPIO_Speed_50MHz;
	gpio.GPIO_OType 	= GPIO_OType_PP;
	gpio.GPIO_PuPd 		= GPIO_PuPd_NOPULL;

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM2);
	GPIO_Init(GPIOA, &gpio);


	/* Configure PA2 as external input (This will be FIELD) */
	GPIO_StructInit(&gpio);

	gpio.GPIO_Pin 		= GPIO_Pin_2;
	gpio.GPIO_Mode 		= GPIO_Mode_IN;
	gpio.GPIO_Speed 	= GPIO_Speed_50MHz;
	gpio.GPIO_OType 	= GPIO_OType_PP;
	gpio.GPIO_PuPd 		= GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &gpio);
}

void initPixelPort()
{
	GPIO_InitTypeDef		GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}

void initPixelClock()
{
	TIM_TimeBaseInitTypeDef		timb;

	TIM_DeInit(TIM8);
	TIM_TimeBaseStructInit(&timb);

	timb.TIM_Prescaler 			= 0;
	timb.TIM_CounterMode 		= TIM_CounterMode_Up;
	timb.TIM_Period 			= pixelClockCycles;
	timb.TIM_ClockDivision 		= TIM_CKD_DIV1;
	timb.TIM_RepetitionCounter 	= 0;
	TIM_TimeBaseInit(TIM8, &timb);

	TIM_SelectSlaveMode(TIM8, TIM_SlaveMode_Trigger);
	TIM_SelectInputTrigger(TIM8, TIM_TS_ETRF);
	TIM_ETRConfig(TIM8, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_Inverted, 0);
	TIM_DMACmd(TIM8, TIM_DMA_Update, ENABLE);
}

void initLineCount()
{
	/* Run TIM2 in external clock mode 2 to count video lines.
	 * ETR (HSYNC on pin PA5) will increment the timer.
	 * TI2FP2 (VSYNC on pin PA1) will reset the timer.
	 * CC1 detects when VBI is over and we need to prepare to output active video.
	 * Interrupt on CC1.
	 */
	TIM_TimeBaseInitTypeDef		timb;
	TIM_OCInitTypeDef			ocnt;
	TIM_ICInitTypeDef 			ici;
	NVIC_InitTypeDef			nvic;

	TIM_TimeBaseStructInit(&timb);
	TIM_OCStructInit(&ocnt);
	TIM_ICStructInit(&ici);
	TIM_DeInit(TIM2);

	timb.TIM_Prescaler 			= 0;
	timb.TIM_CounterMode 		= TIM_CounterMode_Up;
	timb.TIM_Period 			= 65535;
	timb.TIM_ClockDivision 		= TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &timb);

	// External clock mode 2, ETR (HSYNC) will be the clock for TIM2
	TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_Inverted, 0);

	// Detect rising edges on Channel 2 (VSYNC)
	ici.TIM_Channel 		= TIM_Channel_2;
	ici.TIM_ICPolarity 		= TIM_ICPolarity_Rising;
	ici.TIM_ICSelection 	= TIM_ICSelection_DirectTI;
	ici.TIM_ICPrescaler	 	= TIM_ICPSC_DIV1;
	ici.TIM_ICFilter  		= 0;
	TIM_ICInit(TIM2, &ici);

	// TI2FP2 Edge detector will be the signal that will reset the timer
	TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2);
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);

	// OC1 will fire an interrupt when VBI is over
	// Disable output state as we want this in software only
	// OC1 shares a pin with ETR which is in use so we don't want to put the pin into output mode
	ocnt.TIM_OCMode 			= TIM_OCMode_PWM1;
	ocnt.TIM_Pulse 				= activeVideoLineStart;
	ocnt.TIM_OutputState 		= TIM_OutputState_Disable;
	ocnt.TIM_OutputNState 		= TIM_OutputState_Disable;
	ocnt.TIM_OCPolarity 		= TIM_OCPolarity_High;
	ocnt.TIM_OCIdleState 		= TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM2, &ocnt);

	// Interrupt on capture
	nvic.NVIC_IRQChannel 					= TIM2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority 	= 0;
	nvic.NVIC_IRQChannelSubPriority 		= 0;
	nvic.NVIC_IRQChannelCmd 				= ENABLE;

	NVIC_Init(&nvic);

	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

	TIM_Cmd(TIM2, ENABLE);
}

void initPixelDma()
{
	NVIC_InitTypeDef nvic;
	DMA_InitTypeDef	dmai;

	DMA_Cmd(DMA2_Stream1, DISABLE);
	DMA_DeInit(DMA2_Stream1);

	// To be driven by TIM8 Update, we want DMA2 Stream1 Channel 7 (p308, reference manual)
	// Needs to be DMA2 because DMA1 does not have access to the peripherals (http://cliffle.com/article/2015/06/06/pushing-pixels/)
	DMA_StructInit(&dmai);
	dmai.DMA_PeripheralBaseAddr 	= (u32)&GPIOF->ODR;
	dmai.DMA_Memory0BaseAddr 		= (u32)currentOutputBuf;
	dmai.DMA_DIR 					= DMA_DIR_MemoryToPeripheral;
	dmai.DMA_BufferSize 			= FRAME_BUF_WIDTH;
	dmai.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;
	dmai.DMA_MemoryInc 				= DMA_MemoryInc_Enable;
	dmai.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;
	dmai.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte; //DMA_MemoryDataSize_Word;
	dmai.DMA_Mode 					= DMA_Mode_Normal;
	dmai.DMA_Priority 				= DMA_Priority_VeryHigh;
	dmai.DMA_Channel 				= DMA_Channel_7;
	dmai.DMA_FIFOMode 				= DMA_FIFOMode_Disable; //DMA_FIFOMode_Enable;
	dmai.DMA_FIFOThreshold 			= DMA_FIFOThreshold_1QuarterFull; // DMA_FIFOThreshold_Full;	// Should this be DMA_FIFOThreshold_1QuarterFull to reduce contention?
	dmai.DMA_MemoryBurst 			= DMA_MemoryBurst_Single; //DMA_MemoryBurst_INC4;
	dmai.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream1, &dmai);

	nvic.NVIC_IRQChannel 						= DMA2_Stream1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority 		= 1;
	nvic.NVIC_IRQChannelSubPriority 			= 0;
	nvic.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&nvic);

	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
	DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
	DMA_Cmd(DMA2_Stream1, ENABLE);
}

void initPendSV()
{
	// We need to use this function for IRQs with negative values such as PendSV, SVC etc
	// We want PendSV to have the lowest possible priority so it doesn't pre-empt any time-critical interrupts, especially sync
	NVIC_SetPriority(PendSV_IRQn, 255);
}

void INTERRUPT IN_CCM TIM2_IRQHandler()
{
	// Clear pending interrupt(s)
	TIM2->SR = 0;

	// Swap frame buffers and start drawing if we have rendered an entire frame
	if(currentField() != 0)
	{
		if(currentRenderBuf == frameBuf0)
		{
			currentRenderBuf = frameBuf1;
			currentOutputBuf = frameBuf0;
		}
		else
		{
			currentRenderBuf = frameBuf0;
			currentOutputBuf = frameBuf1;
		}

		// Trigger PendSV interrupt to schedule the render of next frame
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	}

	// Begin scanout from the start of the frame buffer
	DMA2_Stream1->M0AR = (u32)currentOutputBuf;

	// Re-enable DMA stream
	DMA2_Stream1->CR |= DMA_SxCR_EN;

	// Re-enable slave mode on TIM8 so that on next HSYNC, TIM8 starts
	TIM8->SMCR = TIM_SlaveMode_Trigger | TIM_TS_ETRF;

	toggleLed1();
}

void INTERRUPT IN_CCM DMA2_Stream1_IRQHandler()
{
	// Clear all interrupt flags
	DMA2->LIFCR = DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1;

	// Disable slave mode, can't stop the timer unless we do this first
	TIM8->SMCR = 0;

	// Stop TIM8, it will be re-enabled by the HSYNC signal
	TIM8->CR1 = 0;
	TIM8->EGR = TIM_EventSource_Update;		// Trigger an Update to reset the counter

	if(currentScanLine() < activeVideoLineEnd)
	{
		// Move DMA source to start of next scanline
		DMA2_Stream1->M0AR += FRAME_BUF_WIDTH;

		// Re-enable DMA for next scanline.
		DMA2_Stream1->CR |= DMA_SxCR_EN;

		// Re-enable slave mode on TIM8 so that on next HSYNC, TIM8 starts
		TIM8->SMCR = TIM_SlaveMode_Trigger | TIM_TS_ETRF;
	}
}

void INTERRUPT IN_CCM PendSV_Handler()
{
	// Reset cycle counter
	DWT->CYCCNT = 0;

	//zerobuf(currentRenderBuf, FRAME_BUF_SIZE);
	memset(currentRenderBuf, 0, FRAME_BUF_SIZE);

	RECT rc = { 1, 10, 298, 100 };
	drawRect(&rc, RED, GRAY);

	RECT rc2 = { 60, 120, 120, 20 };
	drawTestPattern(&rc2);

	// Output cycle count for profiling
	ITM_Port32(1)	= DWT->CYCCNT;
}

void initVideo()
{
	// Clear the buffers
	zerobuf(frameBuf0, FRAME_BUF_SIZE);
	zerobuf(frameBuf1, FRAME_BUF_SIZE);

	// We start outputting buf0, so render to 1
	currentOutputBuf = frameBuf0;
	currentRenderBuf = frameBuf1;

	initRCC();
	initSyncPort();
	initPixelPort();
	initLineCount();
	initPixelClock();
	initPixelDma();
	initPendSV();

	printf("Video configured\r\n");
}
