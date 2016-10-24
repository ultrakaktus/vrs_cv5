/*
 * vrs_cv5.c
 *
 *  Created on: 17. 10. 2016
 *      Author: J
 */
#include "vrs_cv5.h"

/*************** VARIABLES *******************************/

volatile uint16_t value;			//value from ADC
uint8_t buffer[6][2];
uint8_t mode = 0;
uint8_t HalfTX = 0;

/******************* parse measured data to output buffer *************/
void ParseData(void)
{
	static uint8_t HalfTX_old = 1;

	if(HalfTX_old != HalfTX)
	{
		//Send raw ADC value:
		if(mode == 0){
			buffer[0][HalfTX] = value/1000  + '0';
			buffer[1][HalfTX] = (value/100) % 10  + '0';
			buffer[2][HalfTX] = (value/10) % 10  + '0';
			buffer[3][HalfTX] = value % 10  + '0';
			buffer[4][HalfTX] = '\n';
		}
		//Send value in volts:
		else
		{
			uint32_t tmp = (330000 * value)/4095; // tmp je volt * 10^-5
			buffer[0][HalfTX] = (tmp/100000) + '0';
			buffer[1][HalfTX] = ',';
			buffer[2][HalfTX] = (tmp/10000) % 10  + '0';
			buffer[3][HalfTX] = (tmp/1000) % 10  + '0';
			buffer[4][HalfTX] = 'V';
			buffer[5][HalfTX] = '\n';
		}
	}
}


/*************** ADC interrupt init **********************/
void ADC_IRQ_init(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	ADC_ITConfig(ADC1, ADC_IT_OVR, ENABLE);
}

/*************** LED init ********************************/

void led_init(void)
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	/*Configure LED*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*************** ADC init *******************************/

void adc_init(void)
{
  GPIO_InitTypeDef      GPIO_InitStructure;
  ADC_InitTypeDef       ADC_InitStructure;

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//Opravené a upravené

  /* Configure ADCx Channel 2 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable the HSI oscillator */
  RCC_HSICmd(ENABLE);
  /* Check that HSI oscillator is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

  /* Enable ADC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);

  /* ADC1 configuration */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADCx regular channel8 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_384Cycles);

  /* Enable the ADC */
  ADC_Cmd(ADC1, ENABLE);

  /* Wait until the ADC1 is ready */
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {
  }

  /* Start ADC Software Conversion */
  ADC_SoftwareStartConv(ADC1);
}

/*************** ADC interrupt *******************************/

void ADC1_IRQHandler(void)		//startup_stm32l1xx_hd.s
{

	if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
	{
	//if(ADC1->SR & ADC_SR_EOC){
		value = ADC1->DR;
		//ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	}
	else if(ADC_GetFlagStatus(ADC1, ADC_FLAG_OVR)) //if overrun send 'E' (= overrun error)
	{
		USART_SendData(USART1, 'E');
	}
}

/*************** LED blink period *********************/

void blink_delay(uint16_t AD_value)
{
	 static int i = 0;
	 //for(i=0;i<(5000+10*AD_value);i++);
	 if(i>=(500+1*AD_value)){
		 GPIO_ToggleBits(GPIOA, GPIO_Pin_5);		//zmena stavu LED
		 i = 0;
	 }
	 else i++;
}

/*************** UART init ****************************/
void UsartInit(void)
{

	/* PA10->RX PA9->TX */
	GPIO_InitTypeDef      GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* USART1 init */
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);

}

/*************** USART interrupt init *************************/

void USART_IRQ_init(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

/*************** USART interrupt *************************/

void USART1_IRQHandler(void)		//startup_stm32l1xx_hd.s
{
	static uint8_t i=0;
	if(USART1->SR & USART_FLAG_RXNE)
	{
		if(USART_ReceiveData(USART1) == 'm')
			mode = !mode;
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}
	if(USART1->SR & USART_FLAG_TC)
	{
		USART_ClearFlag(USART1, USART_FLAG_TC);
		USART_SendData(USART1, buffer[i][HalfTX]);
		i++;
		if(i>=(5+mode)){
			i=0;
			if(HalfTX)HalfTX = 0;
			else HalfTX = 1;
		}
	}
}
