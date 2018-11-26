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
#define WriteAddress            0x50
#define ReadAddress             0x50


/* Variables ------------------------------------------------------------------*/
uint8_t SlaveAddress; //E0, E1, E2 to ground -> SlaveAddress = B(1010000);
uint8_t TempData;
uint8_t ReadValue;
__IO uint32_t  timeout = LONG_TIMEOUT;
uint8_t Data = 0x69;


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


uint32_t I2C_Write(uint8_t DataToSend, uint16_t WriteAddr)
{
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


int I2C_Read(uint8_t DataReceived, uint16_t ReadAddr)
{
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
  
  /* Slave Address Transmitter */
  I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);
  
  /*!< Test on EV6 and clear it */
  timeout = TIMEOUT;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((timeout--) == 0) return CallBack();
  }
  
  /*!< Send the EEPROM's internal address to read from : only one byte Address */
  I2C_SendData(I2C1, ReadAddr);
  
  /*!< Test on EV8 and clear it */
  timeout = TIMEOUT;  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((timeout--) == 0) return CallBack();
  }
  
  /* Start Condition Again */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /*!< Test on EV5 and clear it Again */
  timeout = TIMEOUT;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((timeout--) == 0) return CallBack();
  }
  
  /* Slave Address Receiver */
  I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Receiver);
  
  /* Wait on ADDR flag to be set */
  timeout = TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET)
  {
    if((timeout--) == 0) return CallBack();
  }
  
  /*!< Disable Acknowledgement */
  I2C_AcknowledgeConfig(I2C1, DISABLE);
  
  /* disable interrupts */
  //__disable_irq();
  
  /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
  (void)I2C1->SR2;
  
  /* Stop Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
  
  /* enable interrupts */
  //__enable_irq();
  
  /* Wait for the byte to be received */
  timeout = TIMEOUT;
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)
  {
    if((timeout--) == 0) return CallBack();
  }
  
  /*!< Read the byte received from the EEPROM */
  DataReceived = I2C_ReceiveData(I2C1);
  
  /* Wait to make sure that STOP control bit has been cleared */
  timeout = TIMEOUT;
  while(I2C1->CR1 & I2C_CR1_STOP)
  {
    if((timeout--) == 0) return CallBack();
  }
  
  /*!< Re-Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  
  return DataReceived;
}


int main()
{
  I2C_DeInit(I2C1);
  I2C1_init();
  
  while(1)
  {
    I2C_Write(Data, WriteAddress);
    ReadValue = I2C_Read(TempData ,ReadAddress);
  }
}
