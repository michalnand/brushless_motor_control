#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

void adc_init();
int32_t adc_read(uint32_t ch);

#endif