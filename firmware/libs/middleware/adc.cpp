#include "adc.h"
#include <device.h>


void adc_init()
{
    ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_DeInit(ADC1);
    
    /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStructure);
    
    /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
    ADC_Init(ADC1, &ADC_InitStructure); 
    
    
    ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_28_5Cycles);


    /* ADC Calibration */
    ADC_GetCalibrationFactor(ADC1);
    
    /* Enable the ADC peripheral */
    ADC_Cmd(ADC1, ENABLE);     
    
    /* Wait the ADRDY flag */
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY))
    {
        __asm("nop");
    }
    
    /* ADC1 regular Software Start Conv */ 
    ADC_StartOfConversion(ADC1);
}

int32_t adc_read(uint32_t ch)
{ 
    ADC_ChannelConfig(ADC1, ch, ADC_SampleTime_28_5Cycles);
    ADC_StartOfConversion(ADC1);

     
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
    {
        __asm("nop");
    }
    
    return ADC_GetConversionValue(ADC1);
}
