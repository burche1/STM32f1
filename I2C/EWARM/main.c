/* Includes -------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "string.h"
#include "stm32f10x_i2c.h"
#include "stdio.h"


/* Macro ----------------------------------------------------------------------*/
#define B(x) S_to_binary_(#x)

static inline unsigned long long S_to_binary_(const char *s)
{
    unsigned long long i = 0;
    while (*s) {
        i <<= 1;
        i += *s++ - '0';
    }
    return i;
}


/* Defines --------------------------------------------------------------------*/
#define TIMEOUT                 ((uint32_t)0x1000)
#define LONG_TIMEOUT            ((uint32_t)(10 * TIMEOUT))


/* Variables ------------------------------------------------------------------*/
uint8_t SlaveAddress; //E0, E1, E2 to ground
uint8_t TempRead;
uint8_t ReadValue;
__IO uint32_t  timeout = LONG_TIMEOUT;


/* Functions ------------------------------------------------------------------*/
uint32_t CallBack(void)
{
  while(1);
}


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


uint32_t I2C_Write(uint8_t* DataToSend, uint16_t WriteAddr, uint8_t NumByteToWrite)
{
  /*!< Address of EEPROM */
  SlaveAddress = B(1010000);
  
  /*!< While the bus is busy */
  timeout = LONG_TIMEOUT;
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    if((timeout--)==0) return CallBack();
  }
  
  /* Start Condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /*!< Test on EV5 and clear it */
  timeout = TIMEOUT;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((timeout--) == 0) return CallBack();
  }
  
  timeout = TIMEOUT;
  /* Slave Address Transmitter */
  I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);
  
  /*!< Test on EV6 and clear it */
  timeout = TIMEOUT;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((timeout--) == 0) return CallBack();
  }
  
  /*!< Send the EEPROM's internal address to write to : only one byte Address */
  I2C_SendData(I2C1, WriteAddr);
  
  /*!< Test on EV8 and clear it */
  timeout = TIMEOUT;  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((timeout--) == 0) return CallBack();
  }  
  
  /*!< Send Data to be written */
  I2C_SendData(I2C1, DataToSend);
  
  /*!< Test on EV8 and clear it */
  timeout = TIMEOUT;  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((timeout--) == 0) return CallBack();
  }
  
  /* Stop Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}


int I2C_Read()
{
  SlaveAddress = B(1010000);
  /* Start Condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* Slave Address Receiver */
  I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Receiver);
  
  /* Ack Test */
  if (I2C_ReceiveData(I2C1) == B(1))
  {
    /* Data */
    TempRead = I2C_ReceiveData(I2C1);      //sucesso da leitura TempRead = 01101001
  }
  else
    TempRead = B(10001111);        //código de erro Address e R/W
  
  /* Ack Test */
  if (I2C_ReceiveData(I2C1) == B(1))
  {
    /* Stop Condition */ 
    I2C_GenerateSTOP(I2C1, ENABLE);
  }
  else
    TempRead = B(10001110);        //código de erro Send Data
  
  return TempRead;
}


int main()
{
  I2C1_init();
  uint8_t Data[] = 01101001;        // i
  TempRead = B(00000000);
  uint8_t WriteAddr = B(00000000);
  uint8_t NumByteToWrite = 1;
  
  while(1)
  {
    I2C_Write(Data, WriteAddr, NumByteToWrite);
    ReadValue = I2C_Read();
  }
}
