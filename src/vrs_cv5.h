/*
 * vrs_cv5.h
 *
 *  Created on: 17. 10. 2016
 *      Author: J
 */

#ifndef VRS_CV5_H_
#define VRS_CV5_H_

#include "stm32l1xx.h"

extern volatile uint16_t value;			//value from ADC

void ADC_IRQ_init(void);				//ADC interrupt init
void USART_IRQ_init(void);				//UART interrupt init
void led_init(void);					//LED GPIO init
void adc_init(void);					//ADC GPIO init
void blink_delay(uint16_t AD_value);	//LED blink period
void UsartInit(void);					//UART GPIO init
void ADC1_IRQHandler(void);				//ADC interrupt
void USART1_IRQHandler(void);			//UART interrupt


#endif /* VRS_CV5_H_ */
