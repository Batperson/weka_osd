/*
 * control.c
 *
 */

#include "stm32f4xx.h"
#include "misc.h"
#include "i2c.h"
#include "bitband.h"
#include "control.h"

void initLeds()
{
	/* Clock to GPIOF */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	/* Initialize PF9, PF10 for LED blinking */
	GPIO_InitTypeDef		GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	setLed1(OFF);
	setLed2(OFF);
}

void initVideoChips()
{
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x00, 0x00);	// Autodetect, CVBS PAL/NTSC/SECAM on AIN1
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x17, 0x41);	// Shaping filter control 1, 1 undocumented setting
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x1D, 0x47);	// Enable 28MHz Crystal
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x19, 0xFA);	// F1 default ** Comb filter control
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x31, 0x02);	// Clear NEWAV_MODE disable, generate SAV/EAV codes to suit ADV video encoders
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x3A, 0x10);	// Set Latch Clock & power up ADC0 - 3
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x3B, 0x71);	// IBIAS ADJUST
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x3D, 0xA2);	// Windowing function 1. MWE Enable Manual Window, Colour Kill Threshold to 2
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x3E, 0x6A);	// Windowing function 2. BLM optimisation
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x3F, 0xA0);	// Windowing function 3. BGB

	// New stuff for SCART
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x4D, 0xEE);	// Disable Chroma Transient Improvement
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x67, 0x01);	// Format 422, undocumented setting
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x73, 0xD0);	// Manual Gain Channels A,B,C , undocumented setting
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x74, 0x04);	// Manual Gain Channels A,B,C , undocumented setting
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x75, 0x01);	// Manual Gain Channels A,B,C , undocumented setting
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x76, 0x00);	// Manual Gain Channels A,B,C , undocumented setting
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x77, 0x04);	// Manual Offsets A to 64d & B,C to 512, undocumented setting
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x78, 0x08);	// Manual Offsets A to 64d & B,C to 512, undocumented setting
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x79, 0x02);	// Manual Offsets A to 64d & B,C to 512, undocumented setting
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x7A, 0x00);	// Manual Offsets A to 64d & B,C to 512, undocumented setting
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xC5, 0x00);	// Clamp Mode 0 for FB hc based, undocumented setting

	I2C_WriteByte(I2C1, ADDR_DECODER, 0xF3, 0x0F);	// Enable Anti Alias Filter on ADC0-3
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xF9, 0x03);	// Set max v lock range
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x0E, 0x80);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x52, 0x46);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x54, 0x00);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x7F, 0xFF);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x81, 0x30);	// ADI Recommended Write,
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x90, 0xC9);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x91, 0x40);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x92, 0x3C);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x93, 0xCA);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x94, 0xD5);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xB1, 0xFF);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xB6, 0x08);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xC0, 0x9A);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xCF, 0x50);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xD0, 0x4E);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xD1, 0xB9);	// ADI Recommended Write. Disable Field output signal generator??
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xD6, 0xDD);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xD7, 0xE2);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xE5, 0x51);	// ADI Recommended Write
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x0E, 0x00);	// ADI Recommended Write Enable ADC step 3???

	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x17, 0x02);	// Software Reset
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x00, 0x10);	// Power up DAC1 only & PLL
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x01, 0x80);	// SD Mode only, Data input on Y-Bus
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x80, 0x11);	// SSAF Luma filter enabled, PAL B/D/G/H/I mode
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x82, 0xCD);	// Step control on, pixel data valid, pedestal on, PrPb SSAF on, CVBS/Pb/Pr on DAC1,2,3
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x87, 0x60);	// PAL/NTSC auto-detect enabled, and one other undocumented bit

	//mine
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x04, 0x65);	// SFL off, ITU-R BT.656-3 compatible
	I2C_WriteByte(I2C1, ADDR_DECODER, 0xF1, 0x01);	// RGB on AIN4,5,6
	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_AD_ENABLE, DEC_AD_PAL_EN | DEC_AD_NTSC_EN | DEC_AD_PALM_EN | DEC_AD_PALN_EN);	// Disable SECAM autodetect

	// Adjust the position of HSYNC, so it reflects the timing of the CVBS input rather than the digital output
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x34, 0x33);	// HS Control
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x35, 0x51);	// HS Control
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x36, 0x50);	// HS Control

	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x88, 0x00);	// 8-bit input

	setFastBlankMode(FBModeDynamicEdgeEnhanced);
	setFastBlankContrastReductionMode(FBContrastReductionEnabled);
	setFastBlankContrastReductionLevel(FBContrastReductionLevel75);
	setFastBlankEdgeShapeLevel(FBEdgeShapeLevel4);
	setFastBlankThresholds(FBLevelThreshold3, FBContrastThreshold3);
}

void showTestPattern()
{
	u8 palFsc[] = { 0xCB, 0x8A, 0x09, 0x2A };

	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x84, 0x40);	// Colour bar test pattern on
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x87, 0x00);	// Disable standard auto-detect
	I2C_BufferWrite(I2C1, ADDR_ENCODER, 0x8C, palFsc, sizeof(palFsc));
}

void setVideoInput(INSELType ain)
{
	u8 inc  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_INP_CONTRL);
	inc		= (inc & 0xFC) | (((u8)ain) & 0x03);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_INP_CONTRL, inc);
}

void setTimingFromPins()
{
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x8A, 0x0C);	// Timing mode 2 (slave) HSYNC / VSYNC pins
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x37, 0xA1);	// HS and VS active-low
}

void set16BitPixelBus()
{
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x03, 0x08);	// 16-bit output
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x27, 0xD8);	// Swap Cr and Cb, pixel delay default
	I2C_WriteByte(I2C1, ADDR_DECODER, 0x37, 0xA1);	// HS and VS active-low
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x01, 0x80);	// SD Mode only, Luma on Y bus, Chroma on C bus
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x8A, 0x0C);	// Timing mode 2 (slave) HSYNC / VSYNC pins
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x88, 0x08);	// 16 bit input enabled
}

void setYPrPbComponentOutput()
{
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x00, 0x1C);	// Power up DAC1,2,3 & PLL
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x02, 0x20);	// YPbPr component output instead of RGB
	I2C_WriteByte(I2C1, ADDR_ENCODER, 0x82, 0xC9);	// Step control on, pixel data valid, pedestal on, PrPb SSAF on, Y/Pb/Pr on DAC1,2,3
}

void setVideoStandard(VIDSELType std)
{
	u8 inc  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_INP_CONTRL);
	inc		= (inc & DEC_INPC_VIDSEL_MASK) | (((u8)std) & DEC_INPC_VIDSEL_MASK);

	I2C_WriteByte(I2C1, ADDR_ENCODER, REG_DEC_INP_CONTRL, inc);
}

void setOutputMode(u8 output)
{

}

void setFreeRunColour(u16 yPbPr)
{
	u8 val  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_DEF_VALUE_Y);
	val 	= (val & ~DEC_FREERUN_Y_MASK) | (((u8)(yPbPr >> 6)) | DEC_FREERUN_Y_MASK);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_DEF_VALUE_C, (u8)yPbPr);
	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_DEF_VALUE_Y, val);
}

void forceFreeRunScreen(FreeRunForceActiveType fa)
{
	u8 val  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_DEF_VALUE_Y);
	val		= (val & ~DEF_VAL_EN) | (fa & DEF_VAL_EN);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_DEF_VALUE_Y, val);
}

void setAntiAliasControl(AFEControlType afe)
{
	u8 v  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_AFE_CONTROL);
	v = (v & ~DEC_AFE_MASK) | (afe & DEC_AFE_MASK);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_AFE_CONTROL, v);
}

void setFastBlankMode(FBModeType fbm)
{
	u8 v  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL1);
	v = (v & ~DEC_FBMODE_MASK) | (fbm & DEC_FBMODE_MASK);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL1, v);
}

void setFastBlankSource(FBSourceType fbs)
{
	u8 v  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL1);
	v = (v & ~DEC_FBSOURCE_MASK) | (fbs & DEC_FBSOURCE_MASK);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL1, v);
}

void setFastBlankAlphaCoefficient(u8 fbac)
{
	u8 v  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL2);
	v = (v & ~DEC_FBALPHA_MASK) | (fbac & DEC_FBALPHA_MASK);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL2, v);
}

void setFastBlankContrastReductionMode(FBContrastReductionModeType fbcm)
{
	u8 v  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL3);
	v = (v & ~DEC_FBCONTRAST_MASK) | (fbcm & DEC_FBCONTRAST_MASK);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL3, v);
}

void setFastBlankEdgeShapeLevel(FBEdgeShapeLevelType fbesl)
{
	u8 v  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL3);
	v = (v & ~DEC_FBEDGESHAPE_MASK) | (fbesl & DEC_FBEDGESHAPE_MASK);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL3, v);
}

void setFastBlankContrastReductionLevel(FBContrastReductionLevelType fbcl)
{
	u8 v  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL5);
	v = (v & ~DEC_FBCONTRASTLEVEL_MASK) | (fbcl & DEC_FBCONTRASTLEVEL_MASK);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL5, v);
}

void setFastBlankThresholds(FBLevelThresholdType fbl, FBContrastThresholdType fbc)
{
	u8 v  = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL5);
	v = (v & ~(DEC_FBLEVELTHRESHOLD_MASK | DEC_FBCONTRASTTHRESHOLD_MASK)) | (fbl & DEC_FBLEVELTHRESHOLD_MASK) | (fbc & DEC_FBCONTRASTTHRESHOLD_MASK);

	I2C_WriteByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL5, v);
}

void setHSyncTiming(u16 hsyncStart, u16 hsyncEnd)
{
	u8 vals[3];
	vals[0]  = (hsyncEnd   & 0x700) >> 8;
	vals[0] |= (hsyncStart & 0x700) >> 4;
	vals[1]  = (hsyncStart & 0xFF);
	vals[2]  = (hsyncEnd   & 0xFF);

	I2C_BufferWrite(I2C1, ADDR_DECODER, REG_DEC_HSE, vals, 3);
}
