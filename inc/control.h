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
	IN_LOCK								= 0x01,
	LOST_LOCK							= 0x02,
	FSC_LOCK							= 0x04,
	FOLLOW_PW							= 0x08,
	ADMask								= 0x70,
	AD_NTSC_MJ 							= 0x00,
	AD_NTSC_443							= 0x10,
	AD_PAL_M							= 0x20,
	AD_PAL_60							= 0x30,
	AD_PAL_BGHID						= 0x40,
	AD_SECAM							= 0x50,
	AD_PAL_CMB_N						= 0x60,
	AD_SECAM_525						= 0x70,
	COL_KILL							= 0x80
} Status1Type;

typedef enum {
	MVCS_DET							= 0x01,
	MVCS_T3								= 0x02,
	MV_PS_DET							= 0x04,
	MV_AGC_DET							= 0x08,
	LL_NSTD								= 0x10,
	FSC_NSTD							= 0x20,
} Status2Type;

typedef enum {
	INST_HLOCK							= 0x01,
	GEMD								= 0x02,
	SD_OP_50HZ							= 0x04,
	CVBS								= 0x08,
	FREE_RUN_ACT						= 0x10,
	STD_FLD_LEN							= 0x20,
	INTERLACED							= 0x40,
	PAL_SW_LOCK							= 0x80
} Status3Type;

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

typedef enum {
	PrPbSSAFDisabled					= 0x00,
	PrPbSSAFEnabled						= 0x01,
} PrPbSSAFEnabledType;

typedef enum {
	LowPowerDAC1						= 0x01,
	LowPowerDAC2						= 0x02,
	LowPowerDAC3						= 0x04,
} EncoderLowPowerModeType;

typedef enum {
	UserSubMapDisabled					= 0x00,
	UserSubMapEnabled					= 0x20
} UserSubMapType;

typedef enum {
	DriveOpenDrain						= 0x00,
	DriveActiveLow						= 0x01,
	DriveActiveHigh						= 0x02,
	ManualInterruptDisabled				= 0x00,
	ManualInterruptEnabled				= 0x04,
	MacrovisionPseudoSyncOnly			= 0x10,
	MacrovisionColourStripeOnly			= 0x20,
	MacrovisionPseudoSyncAndColour		= 0x30,
	Duration3Xtal						= 0x00,
	Duration15Xtal						= 0x40,
	Duration63Xtal						= 0x80,
	DurationActiveUntilCleared			= 0xC0
} InterruptConfigType;

typedef enum {
	Lock	 							= 0x01,
	Unlock								= 0x02,
	FreeRun								= 0x20,
	MVPseudoSyncColourStripe			= 0x40,
	Interrupt1All						= 0xFF
} Interrupt1Type;

typedef enum {
	SDStandard 							= 0x01,
	SDVerticalLock						= 0x02,
	SDHorizontalLock					= 0x04,
	SDAutodetectResult					= 0x08,
	SDSECAMLock							= 0x10,
	SDPALSwingBurstLock					= 0x20,
	Interrupt3All						= 0xFF
} Interrupt3Type;

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
void setPrPbSSAFEnabled(PrPbSSAFEnabledType enable);
void setLowPowerMode(EncoderLowPowerModeType enable);

void setUserSubMap(UserSubMapType usr);

void setInterruptConfig(InterruptConfigType config);
void setInterruptClear(Interrupt1Type clr1, Interrupt3Type clr3);

Interrupt1Type getInterrupt1ChangeStatus();
void setInterrupt1Clear(Interrupt1Type clr);
void setInterrupt1Mask(Interrupt1Type msk);
Interrupt3Type getInterrupt3RawStatus();
Interrupt3Type getInterrupt3ChangeStatus();
void setInterrupt3Clear(Interrupt3Type clr);
void setInterrupt3Mask(Interrupt3Type msk);

Status1Type getDecoderStatus1();
Status2Type getDecoderStatus2();
Status3Type getDecoderStatus3();

void setDecoderCtiEnabled(u8 enable);
void setDecoderCtiAlphaBlendEnabled(u8 enable);
void setDecoderDnrEnabled(u8 enable);
void setDecoderCtiChromaTheshold(u8 threshold);
void setDecoderDnrNoiseTheshold(u8 threshold);

typedef enum {
	OFF 	= 0x00,
	ON 		= 0x01
} LEDStatusType;

ALWAYS_INLINE void setLed1 (LEDStatusType status)
{
	OUTPUT_PIN(GPIOB, 0) = status;
}

ALWAYS_INLINE void setLed2(LEDStatusType status)
{
	OUTPUT_PIN(GPIOB, 7) = status;
}

ALWAYS_INLINE void setLed3(LEDStatusType status)
{
	OUTPUT_PIN(GPIOB, 14) = status;
}

ALWAYS_INLINE void toggleLed1()
{
	OUTPUT_PIN(GPIOB, 0) = !OUTPUT_PIN(GPIOB, 0);
}

ALWAYS_INLINE void toggleLed2()
{
	OUTPUT_PIN(GPIOB, 7) = !OUTPUT_PIN(GPIOB, 7);
}

ALWAYS_INLINE void toggleLed3()
{
	OUTPUT_PIN(GPIOB, 14) = !OUTPUT_PIN(GPIOB, 14);
}

#endif /* CONTROL_H_ */
