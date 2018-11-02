/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"

void RCC_Configuration(void);
void GPIO_Configuration(void);
void Delay(uint32_t nTime);
int uart_send(USART_TypeDef* USARTx, char c);

int main(void)
{
  /* Function to verify System Clock */
  static RCC_ClocksTypeDef test_clk;
  RCC_GetClocksFreq(&test_clk);
  
  /* System Clocks Configuration */
  RCC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();  

/* USARTy and USARTz configuration ------------------------------------------------------*/
  /* USARTy and USARTz configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitTypeDef USART_InitStructure;
  USART_StructInit(&USART_InitStructure);
  
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);
  
  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);
  
  char *stringToSend = "Hello World\r\n";
  
  while(1)
  {
    int i;
    for (i = 0; i < strlen(stringToSend); i++)
      uart_send(USART1, stringToSend[i]);
    Delay(7200000);
  }
}


/* Timer */
static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0)
  {
    TimingDelay--;
  }
}

/* USART Send */
int uart_send(USART_TypeDef* USARTx, char c)
{
  while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
  USART_SendData(USARTx, c);
  
  return 0;
}

/* Clock Config */
void RCC_Configuration(void)
{    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | 
                         RCC_APB2Periph_AFIO |
                         RCC_APB2Periph_GPIOA, ENABLE);
}

/* GPIO Config */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_StructInit(&GPIO_InitStructure);
  
  //Initialize USART1_Tx
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //Initialize USART1_Rx
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
}