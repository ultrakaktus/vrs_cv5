/*
 * vrs_cv5.h
 *
 *  Created on: 17. 10. 2016
 *      Author: J
 */

#ifndef VRS_CV5_H_
#define VRS_CV5_H_

#include "stm32l1xx.h"

void ADC_IRQ_init(void);
void led_init(void);
void adc_init(void);
void blink_delay(uint16_t AD_value);

#endif /* VRS_CV5_H_ */
