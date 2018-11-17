/* Includes -------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "string.h"
#include "stm32f10x_i2c.h"
#include "stdio.h"


/* Variables ------------------------------------------------------------------*/
uint8_t SlaveAddress = 1010000;
uint8_t Read = 1;
uint8_t Write = 0;
uint8_t Data = 01101001;        // i


/* Functions ------------------------------------------------------------------*/

void I2C1_init(void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* Configure I2C_EE pins: SCL and SDA */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* I2C configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x38;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;

    /* I2C Peripheral Enable */
    I2C_Cmd(I2C1, ENABLE);
    /* Apply I2C configuration after enabling it */
    I2C_Init(I2C1, &I2C_InitStructure);
}

/*
  * @brief  Configures the specified I2C own address2.
  * @param  I2Cx: where x can be 1 or 2 to select the I2C peripheral.
  * @param  Address: specifies the 7bit I2C own address2.
  * @retval None.
I2C_OwnAddress2Config(I2C_TypeDef* I2Cx, uint8_t Address)
*/


/*
  * @brief  Sends a data byte through the I2Cx peripheral.
  * @param  I2Cx: where x can be 1 or 2 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval None
I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data)
*/


/*
  * @brief  Returns the most recent received data by the I2Cx peripheral.
  * @param  I2Cx: where x can be 1 or 2 to select the I2C peripheral.
  * @retval The value of the received data.
I2C_ReceiveData(I2C_TypeDef* I2Cx)
*/


/*
  * @brief  Transmits the address byte to select the slave device.
  * @param  I2Cx: where x can be 1 or 2 to select the I2C peripheral.
  * @param  Address: specifies the slave address which will be transmitted
  * @param  I2C_Direction: specifies whether the I2C device will be a
  *   Transmitter or a Receiver. This parameter can be one of the following values
  *     @arg I2C_Direction_Transmitter: Transmitter mode
  *     @arg I2C_Direction_Receiver: Receiver mode
  * @retval None.
I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction)
*/


int main()
{
  I2C1_init();
  /* Start Condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Slave Address Transmitter */
  //I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);
  
  /* Slave Address Receiver */
  //I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Receiver);
  
  /* Read or Write */
  //I2C_SendData(I2C1, Read);
  //I2C_SendData(I2C1, Write);
  
  /* Ack */
  //Ack
  
  /* Data */
  //I2C_SendData(I2C1, Data);
  
  /* Ack */
  //Ack
  
  /* Stop Condition */ 
  I2C_GenerateSTOP(I2C1, ENABLE);
  
  while(1);
}
