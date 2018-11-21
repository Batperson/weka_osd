/*
 * i2c.h
 * Low-level I2C hardware communication
 */

#ifndef I2C_H_
#define I2C_H_

// Decoder defines
#define ADDR_DECODER					0x42

#define REG_DEC_INP_CONTRL				0x00
#define REG_DEC_AD_ENABLE				0x07
#define REG_DEC_DEF_VALUE_Y				0x0C
#define REG_DEC_DEF_VALUE_C				0x0D
#define REG_DEC_STATUS1					0x10
#define REG_DEC_IDENT					0x11
#define REG_DEC_STATUS2					0x12
#define REG_DEC_STATUS3					0x13
#define REG_DEC_AFE_CONTROL				0xF3
#define REG_DEC_FB_CONTROL1				0xED
#define REG_DEC_FB_CONTROL2				0xEE
#define REG_DEC_FB_CONTROL3				0xEF
#define REG_DEC_FB_CONTROL4				0xF0
#define REG_DEC_FB_CONTROL5				0xF1

#define DEC_AFE_MASK					0x0F
#define DEC_FBMODE_MASK					0x03
#define DEC_FBSOURCE_MASK				0x04
#define DEC_FBALPHA_MASK				0x7F
#define DEC_FBEDGESHAPE_MASK			0x07
#define DEC_FBCONTRAST_MASK				0x08
#define DEC_FBCONTRASTLEVEL_MASK		0x0C
#define DEC_FBLEVELTHRESHOLD_MASK		0x30
#define DEC_FBCONTRASTTHRESHOLD_MASK	0xC0
#define DEC_INPC_INSEL_MASK				0x0F
#define DEC_INPC_VIDSEL_MASK			0xF0
#define DEC_INPC_VIDSEL_ALL				0x00
#define DEC_FREERUN_Y_MASK				0xFC

#define DEC_STATUS1_IN_LOCK				0x01
#define DEC_STATUS1_LOST_LOCK			0x02
#define DEC_STATUS1_FSC_LOCK			0x04
#define DEC_STATUS1_FOLLOW_PW			0x08
#define DEC_STATUS1_AD_MASK				0x70
#define DEC_STATUS1_COLORKILL			0x80

#define DEC_STATUS2_MVCS_DET			0x01
#define DEC_STATUS2_MVCS_T3				0x02
#define DEC_STATUS2_MV_PS_DET			0x04
#define DEC_STATUS2_MV_AGC_DET			0x08
#define DEC_STATUS2_LL_NSTD				0x10
#define DEC_STATUS2_FSC_NSTD			0x20

#define DEC_STATUS3_INST_HLOCK			0x01
#define DEC_STATUS3_GEMD				0x02
#define DEC_STATUS3_SD_OP_50HZ			0x04
#define DEC_STATUS3_CVBS				0x08
#define DEC_STATUS3_FREE_RUN_ACT		0x10
#define DEC_STATUS3_STD_FLD_LEN			0x20
#define DEC_STATUS3_INTERLACED			0x40
#define DEC_STATUS3_PAL_SW_LOCK			0x80

#define DEF_VAL_EN						0x01
#define DEF_VAL_AUTO_EN					0x02

#define DEC_AD_PAL_EN					0x01
#define DEC_AD_NTSC_EN					0x02
#define DEC_AD_PALM_EN					0x04
#define DEC_AD_PALN_EN					0x08
#define DEC_AD_P60_EN					0x10
#define DEC_AD_N443_EN					0x20
#define DEC_AD_SECAM_EN					0x40
#define DEC_AD_SEC525_EN				0x80

#define REG_DEC_HSE						0x34
#define REG_DEC_PIN_POLARITY			0x37
#define REG_DEC_CTI_DNR_1				0x4d
#define REG_DEC_CTI_DNR_2				0x4e
#define REG_DEC_CTI_DNR_4				0x50

// Encoder defines
#define ADDR_ENCODER					0x56

#define REG_ENC_MODE					0x00
#define REG_ENC_CABLE_DETECT			0x10
#define REG_ENC_DNR_0					0xA3
#define REG_ENC_DNR_1					0xA4
#define REG_ENC_DNR_2					0xA5
#define REG_ENC_SD_MODE_1				0x80
#define REG_ENC_SD_MODE_7				0x88

void initI2C1();
u8 I2C_ReadByte(I2C_TypeDef* pI2c, u8 slaveAddr, u8 ReadAddr);
void I2C_WriteByte(I2C_TypeDef* I2Cx, u8 slaveAddr, u8 writeAddr, u8 data);
void I2C_BufferRead(I2C_TypeDef* pI2c, u8 slaveAddr, u8 ReadAddr, u8* pBuffer, u16 NumByteToRead);
void I2C_BufferWrite(I2C_TypeDef* I2Cx, u8 slaveAddr, u8 WriteAddr, u8* pBuffer, u16 numByteToWrite);


#endif /* I2C_H_ */
