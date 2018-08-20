/*
 * i2c.c
 *
 */

#include "stm32f4xx.h"
#include "i2c.h"

void initI2C1()
{
	I2C_InitTypeDef  iic;
	GPIO_InitTypeDef  gp;

	GPIO_StructInit(&gp);
	I2C_StructInit(&iic);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* Configure I2C_EE pins: SCL and SDA */
	gp.GPIO_Pin 				= GPIO_Pin_6 | GPIO_Pin_7;
	gp.GPIO_Speed 				= GPIO_Speed_50MHz;
	gp.GPIO_Mode 				= GPIO_Mode_AF;
	gp.GPIO_OType 				= GPIO_OType_OD;
	gp.GPIO_PuPd  				= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &gp);

	/* Note GPIO_PinSourceN not GPIO_Pin_N */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

	/* I2C configuration */
	iic.I2C_Mode 				= I2C_Mode_I2C;
	iic.I2C_DutyCycle 			= I2C_DutyCycle_2;
	iic.I2C_OwnAddress1 		= 0x00;	// Own address only relevant if we are a slave
	iic.I2C_Ack 				= I2C_Ack_Enable;
	iic.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	iic.I2C_ClockSpeed 			= 400000;

	I2C_Init(I2C1, &iic);

	/* Enable after configuring (changed order from before) */
	I2C_Cmd(I2C1, ENABLE);
}

/**
 * @brief  Reads one byte from the  device.
 * @param  I2Cx : I2C peripheral.
 * @param  slaveAddr  : slave address
 * @param  readAddr   : address of the register from which to read
 * @retval The value read from the device
 */
u8 I2C_ReadByte(I2C_TypeDef* I2Cx, u8 slaveAddr, u8 readAddr)
{
    u8 tmp = 0;
    I2C_BufferRead(I2Cx, slaveAddr, readAddr, &tmp, 1);
    return tmp;
}

/**
 * @brief  Reads a block of data from the device.
 * @param  I2Cx : I2C peripheral.
 * @param  slaveAddr  : slave address
 * @param  pBuffer : pointer to the buffer that receives the data read from the device.
 * @param  readAddr : device's internal address to read from.
 * @param  numByteToRead : number of bytes to read from the device.
 * @retval None
 */
void I2C_BufferRead(I2C_TypeDef* I2Cx, u8 slaveAddr, u8 readAddr, u8* pBuffer, u16 numByteToRead)
{
    /* While the bus is busy */
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

    /* Send START condition */
    I2C_GenerateSTART(I2Cx, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send  address for write */
    I2C_Send7bitAddress(I2Cx, slaveAddr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(I2Cx, ENABLE);

    /* Send the internal address to write to */
    I2C_SendData(I2Cx, readAddr);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send START condition a second time */
    I2C_GenerateSTART(I2Cx, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send address for read */
    I2C_Send7bitAddress(I2Cx, slaveAddr, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* While there is data to be read */
    while (numByteToRead)
    {
        if (numByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(I2Cx, DISABLE);

            /* Send STOP Condition */
            I2C_GenerateSTOP(I2Cx, ENABLE);
        }

        /* Test on EV7 and clear it */
        if (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            /* Read a byte from the device */
            *pBuffer = I2C_ReceiveData(I2Cx);

            /* Point to the next location where the byte read will be saved */
            pBuffer++;

            /* Decrement the read bytes counter */
            numByteToRead--;
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
}

/**
 * @brief  Writes one byte to the  device.
 * @param  I2Cx : I2C peripheral.
 * @param  slaveAddr : slave address.
 * @param  WriteAddr : address of the register in which the data will be written
 * @param  pBuffer : pointer to the buffer  containing the data to be written to the device.
 * @retval None
 */
void I2C_WriteByte(I2C_TypeDef* I2Cx, u8 slaveAddr, u8 writeAddr, u8 data)
{
    /* Send START condition */
    I2C_GenerateSTART(I2Cx, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send HMC5883 address for write */
    I2C_Send7bitAddress(I2Cx, slaveAddr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Send the device internal address to write to */
    I2C_SendData(I2Cx, writeAddr);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send the byte to be written */
    I2C_SendData(I2Cx, data);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STOP condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
}

void I2C_BufferWrite(I2C_TypeDef* I2Cx, u8 slaveAddr, u8 WriteAddr, u8* pBuffer, u16 numByteToWrite)
{
    /* Send START condition */
    I2C_GenerateSTART(I2Cx, ENABLE);

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send HMC5883 address for write */
    I2C_Send7bitAddress(I2Cx, slaveAddr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Send the device internal address to write to */
    I2C_SendData(I2Cx, WriteAddr);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    while (numByteToWrite--)
	{
    	/* Send the byte to be written */
		I2C_SendData(I2Cx, *pBuffer++);

		/* Test on EV8 and clear it */
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}

    /* Send STOP condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
}
