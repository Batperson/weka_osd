/*
 * control.h
 * High-level hardware control
 */

#ifndef CONTROL_H_
#define CONTROL_H_

typedef enum {
	AIN1_CVBS							= 0x00,
	AIN2_CVBS							= 0x01,
	AIN3_CVBS							= 0x02,
} INSELType;

typedef enum {
	VS_AD_PAL_BGHID_NTSC_NOPDSTL_SECAM	= 0x00,
	VS_AD_PAL_BGHID_NTSC_M_SECAM		= 0x10,
	VS_AD_PAL_N_NTSC_M_NOPDSTL_SECAM	= 0x20,
	VS_AD_PAL_N_NTSC_M_SECAM			= 0x30,
	VS_NTSC_J							= 0x40,
	VS_NTSC_M							= 0x50,
	VS_PAL_60							= 0x60,
	VS_NTSC_443							= 0x70,
	VS_PAL_BGHID						= 0x80,
	VS_PAL_N							= 0x90,
	VS_PAL_M_NOPDSTL					= 0xA0,
	VS_PAL_M							= 0xB0,
	VS_PAL_CMB_N						= 0xC0,
	VS_PAL_CMB_N2						= 0xD0,
	VS_SECAM_NOPDSTL					= 0xE0,
	VS_SECAM_NODPSTL2					= 0xF0
} VIDSELType;

typedef enum {
	AD_NTSC_MJ 							= 0x00,
	AD_NTSC_443							= 0x10,
	AD_PAL_M							= 0x20,
	AD_PAL_60							= 0x30,
	AD_PAL_BGHID						= 0x40,
	AD_SECAM							= 0x50,
	AD_PAL_CMB_N						= 0x60,
	AD_SECAM_525						= 0x70
} ADResultType;

typedef enum {
	AAFiltEn0						 	= 0x01,
	AAFiltEn1						 	= 0x02,
	AAFiltEn2						 	= 0x04,
	AAFiltEn3						 	= 0x08,
} AFEControlType;

typedef enum {
	FBModeStatic						= 0x00,
	FBModeFixedAlpha					= 0x01,
	FBModeDynamic						= 0x02,
	FBModeDynamicEdgeEnhanced			= 0x03
} FBModeType;

typedef enum {
	FBSourceCvbs						= 0x00,
	FBSourceRgb							= 0x04
} FBSourceType;

typedef enum {
	FBContrastReductionDisabled			= 0x00,
	FBContrastReductionEnabled			= 0x08
} FBContrastReductionModeType;

typedef enum {
	FBContrastReductionLevel25			= 0x00,
	FBContrastReductionLevel50			= 0x04,
	FBContrastReductionLevel75			= 0x08,
	FBContrastReductionLevel100			= 0x0C
} FBContrastReductionLevelType;

typedef enum {
	FBEdgeShapeNone						= 0x00,
	FBEdgeShapeLevel1					= 0x01,
	FBEdgeShapeLevel2					= 0x02,
	FBEdgeShapeLevel3					= 0x03,
	FBEdgeShapeLevel4					= 0x04,
} FBEdgeShapeLevelType;

typedef enum {
	FBLevelThreshold1					= 0x00,
	FBLevelThreshold2					= 0x10,
	FBLevelThreshold3					= 0x20,
	FBLevelThreshold4					= 0x30,
} FBLevelThresholdType;

typedef enum {
	FBContrastThreshold1				= 0x00,
	FBContrastThreshold2				= 0x40,
	FBContrastThreshold3				= 0x80
} FBContrastThresholdType;

typedef enum {
	FreeRunDefault						= 0x00,
	FreeRunForceActive				 	= 0x01
} FreeRunForceActiveType;

typedef enum {
	DNRDisabled							= 0x00,
	DNREnabled							= 0x20
} DNREnabledType;

typedef enum {
	DNRBorderTwoPixels					= 0x00,
	DNRBorderFourPixels					= 0x40
} DNRBorderAreaType;

typedef enum {
	DNRBlockEightPixels					= 0x00,
	DNRBlockSixteenPixels				= 0x80
} DNRBlockSizeType;

typedef enum {
	DNRModeNormal						= 0x00,
	DNRModeSharpness					= 0x08
} DNRModeType;

typedef enum {
	DNRFilterNone						= 0x00,
	DNRFilterA							= 0x01,
	DNRFilterB							= 0x02,
	DNRFilterC							= 0x03,
	DNRFilterD							= 0x04
} DNRFilterType;

typedef enum {
	LumaLPFNTSC							= 0x00,
	LumaLPFPAL							= 0x04,
	LumaNotchNTSC						= 0x08,
	LumaNotchPAL						= 0x0C,
	LumaSSAF							= 0x10,
	LumaCIF								= 0x14,
	LumaQCIF							= 0x18,
	LumaReserved						= 0x1C
} LumaFilterType;

typedef enum {
	Chroma1_3MHz						= 0x00,
	Chroma0_65MHz						= 0x20,
	Chroma1_0MHz						= 0x40,
	Chroma2_0MHz						= 0x60,
	ChromaReserved						= 0x80,
	ChromaCIF							= 0xA0,
	ChromaQCIF							= 0xC0,
	Chroma3_0MHz						= 0xE0
} ChromaFilterType;

void initLeds();
void initUserButtons();
void initVideoChips();
void showTestPattern();
void setVideoInput(INSELType ain);
void setTimingFromPins();
void setYPrPbComponentOutput();
void setVideoStandard(VIDSELType std);
void setOutputMode(u8 output);
void setFreeRunColour(u16 yPbPr);
void forceFreeRunScreen(FreeRunForceActiveType fa);
void setAntiAliasControl(AFEControlType afe);
void setFastBlankMode(FBModeType fbm);
void setFastBlankSource(FBSourceType fbs);
void setFastBlankAlphaCoefficient(u8 fbac);
void setFastBlankContrastReductionMode(FBContrastReductionModeType fbcm);
void setFastBlankContrastReductionLevel(FBContrastReductionLevelType fbcl);
void setFastBlankEdgeShapeLevel(FBEdgeShapeLevelType fbesl);
void setFastBlankThresholds(FBLevelThresholdType fbl, FBContrastThresholdType fbc);
void setHSyncTiming(u16 hsyncStart, u16 hsyncEnd);

void setDnrEnabled(DNREnabledType dnrEnable);
void setDnrGain(u8 coringGainBorder, u8 coringGainData);
void setDnrThreshold(u8 threshold, DNRBorderAreaType borderSize, DNRBlockSizeType blockSize);
void setDnrFilter(DNRFilterType filter);
void setDnrMode(DNRModeType mode);
void setDnrBlockOffset(u8 offset);
void setLumaFilter(LumaFilterType filter);
void setChromaFilter(ChromaFilterType filter);


void setDecoderCtiEnabled(u8 enable);
void setDecoderCtiAlphaBlendEnabled(u8 enable);
void setDecoderDnrEnabled(u8 enable);
void setDecoderCtiChromaTheshold(u8 threshold);
void setDecoderDnrNoiseTheshold(u8 threshold);

// The LEDs are effectively active-low because they are connected to VCC and the GPIO is GND. Set to 0 to turn the LEDs on.
typedef enum {
	ON 	= 0x00,
	OFF = 0x01
} LEDStatusType;

ALWAYS_INLINE void setLed1 (LEDStatusType status)
{
	OUTPUT_PIN(GPIOF, 9) = status;
}

ALWAYS_INLINE void setLed2(LEDStatusType status)
{
	OUTPUT_PIN(GPIOF, 10) = status;
}

ALWAYS_INLINE void toggleLed1()
{
	OUTPUT_PIN(GPIOF, 9) = !OUTPUT_PIN(GPIOF, 9);
}

ALWAYS_INLINE void toggleLed2()
{
	OUTPUT_PIN(GPIOF, 10) = !OUTPUT_PIN(GPIOF, 10);
}

#endif /* CONTROL_H_ */
