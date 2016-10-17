/*
 * vrs_cv5.c
 *
 *  Created on: 17. 10. 2016
 *      Author: J
 */
#include "vrs_cv5.h"

extern volatile uint16_t value;

void ADC_IRQ_init(void){
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}
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
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_16Cycles);

  /* Enable the ADC */
  ADC_Cmd(ADC1, ENABLE);

  /* Wait until the ADC1 is ready */
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {
  }

  /* Start ADC Software Conversion */
  ADC_SoftwareStartConv(ADC1);
}
void ADC1_IRQHandler(void)		//startup_stm32l1xx_hd.s
{
	if(ADC1->SR & ADC_SR_EOC)
		value = ADC1->DR;
}
void blink_delay(uint16_t AD_value)
{
	 int i = 0;
	 for(i=0;i<(5000+10*AD_value);i++);
}
