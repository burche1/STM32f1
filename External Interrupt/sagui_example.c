/*****************************************************************************************************/

/* Includes libraries */
#include <misc.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"

/*****************************************************************************************************/

/* Variables */
uint16_t uiLedTm = 0;
enLedState = 0u;
int count = 0;

/*****************************************************************************************************/

/* Main function */

void main (void)
{
    /* Configuration functions */
    fnvConfigNVIC();
    fnvConfigRCC();
    fnvConfigTimer();
    fnvConfigIntExt();
    fnvConfigGPIO();
    
    /* Infinite loop */
    while (1)
    {        
        fnvLedCtrl();
        fnvInputDeb();
    }
}

/*****************************************************************************************************/

/* Control function */

void fnvLedCtrl(void)
{
    if (uiLedTm == 0)
    {
        switch (enLedState)
        {
            /*IDLE - Sempre desligado.*/
            case 0:
            {
                LED desligado;
            }
            break;
            case 1:
            {
                LED ligado;
                
                uiLedTm = 1000;
                
                enLedState = 2;
            }
            break;
            case 2:
            {
                enLedState = 0;
            }
        }
    }
}

/*****************************************************************************************************/

/* Handlers */

void TIM2_IRQHandler(void)
{
    if (uiLedTm != 0)
    {
        uiLedTm--;
    }
    
    if (uiInDebTm != 0)
    {
        uiInDebTm--;
    }
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}

void EXTI4_IRQHandler(void)
{
        if (enLedState == 0)
        {
            enLedState = 1;
        }
        EXTI_ClearITPendingBit(EXTI_Line4);
        count++;
}

/*****************************************************************************************************/

/* Debounce function */

uint8_t ucIgnDebCnt;
uint8_t ucIgnPrevValue;
uint8_t ucIgnValue;

void fnvInputDeb()
{
    if (uiInDebTm == 0u)
    {
        uiInDebTm = 10u;
        
        
        
    }
}

/*****************************************************************************************************/

/* Declaration of configuration functions */

void fnvConfigGPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    /* Configure PC.13 pin as PP */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
  
    /* Configure PA.04 pin as in floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);
}

void fnvConfigTimer(void)
{
    TIM_TimeBaseInitTypeDef TIMER_InitStructure;
        
	TIM_TimeBaseStructInit(&TIMER_InitStructure);
	TIMER_InitStructure.TIM_Prescaler = 65535; 
	TIMER_InitStructure.TIM_Period = 732; 
	TIMER_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIMER_InitStructure);
        
    /* 
    This next line needs to be put here!!!
    That's why the TIM_TimeBaseInit(TIM2, &TIMER_InitStructure); line sets the UIF flag
    We are clearing it!!!
    */
        
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
	TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single);        //one time
    //TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Repetitive);  //repetitive 

    // Enable Timer Interrupt , enable timer
	TIM_ITConfig(TIM2, TIM_IT_Update , ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void fnvConfigIntExt(void)
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    /* Configure EXTI4 line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void fnvConfigRCC(void)
{
    /* Enable GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    /* Enable GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /* Enable AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void fnvConfigNVIC(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitTypeDef   NVIC_InitStructure;
    /* Enable and set EXTI4 Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
