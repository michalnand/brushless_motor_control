#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Memory locations defined by the linker
extern uint32_t _estack[];
extern uint32_t _sdata[], _edata[];
extern uint32_t _etext[];                // End of code/flash
extern void (*__init_array_start)();     //constructors
extern void (*__init_array_end)();

extern int main(void);

// Default interrupt handler
void __attribute__((interrupt("IRQ"))) Default_Handler()
{
    
}

void Reset_Handler() __attribute__((naked, aligned(2)));
/* Weak definitions of handlers point to Default_Handler if not implemented */
void NMI_Handler()                      __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler()                __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler()                __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler()                 __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler()               __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler()                 __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler()                   __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler()                      __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler()                  __attribute__ ((weak, alias("Default_Handler")));


void WWDG_IRQHandler()                  __attribute__ ((weak, alias("Default_Handler")));
void PVD_IRQHandler()                   __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler()                   __attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler()                 __attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler()                   __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_1_IRQHandler()               __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_3_IRQHandler()               __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_15_IRQHandler()              __attribute__ ((weak, alias("Default_Handler")));
void TS_IRQHandler()                    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler()         __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel2_3_IRQHandler()       __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel4_5_IRQHandler()       __attribute__ ((weak, alias("Default_Handler")));
void ADC1_COMP_IRQHandler ()            __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_UP_TRG_COM_IRQHandler()   __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler()               __attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler()                  __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler()                  __attribute__ ((weak, alias("Default_Handler")));
void TIM6_DAC_IRQHandler()              __attribute__ ((weak, alias("Default_Handler")));
void TIM14_IRQHandler()                 __attribute__ ((weak, alias("Default_Handler")));
void TIM15_IRQHandler()                 __attribute__ ((weak, alias("Default_Handler")));
void TIM16_IRQHandler()                 __attribute__ ((weak, alias("Default_Handler")));
void TIM17_IRQHandler()                 __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler()                  __attribute__ ((weak, alias("Default_Handler")));
void I2C2_IRQHandler()                  __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler()                  __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler()                  __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler()                __attribute__ ((weak, alias("Default_Handler")));
void USART2_IRQHandler()                __attribute__ ((weak, alias("Default_Handler")));
void CEC_IRQHandler()                   __attribute__ ((weak, alias("Default_Handler")));
void BootRAM()                          __attribute__ ((weak, alias("Default_Handler")));



// ----------------------------------------------------------------------------------
// Interrupt vector table (loaded into flash memory at 0x000)
//
void (* const InterruptVector[])() __attribute__ ((section(".isr_vector"), aligned(2))) = {
    (void(*)(void)) (int)_estack,                // Initial stack pointer
    Reset_Handler,                                    // Reset handler
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,

    WWDG_IRQHandler,
    PVD_IRQHandler,
    RTC_IRQHandler,
    FLASH_IRQHandler,
    RCC_IRQHandler,
    EXTI0_1_IRQHandler,
    EXTI2_3_IRQHandler,
    EXTI4_15_IRQHandler,
    TS_IRQHandler,
    DMA1_Channel1_IRQHandler,
    DMA1_Channel2_3_IRQHandler,
    DMA1_Channel4_5_IRQHandler,
    ADC1_COMP_IRQHandler ,
    TIM1_BRK_UP_TRG_COM_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM6_DAC_IRQHandler,
    0,
    TIM14_IRQHandler,
    TIM15_IRQHandler,
    TIM16_IRQHandler,
    TIM17_IRQHandler,
    I2C1_IRQHandler,
    I2C2_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    0,
    CEC_IRQHandler,
    0,
    BootRAM
};

#include "device.h"

void Reset_Handler(void)
{
    // copy values to initialize data segment
    uint32_t *fr        = _etext;
    uint32_t *to        = _sdata;
    unsigned int len    = _edata - _sdata;

    while(len--) 
    {
        *to++ = *fr++;
    }

        
    //call global constructors
    void (**p)() = &__init_array_start;
    for (int i = 0; i < (&__init_array_end - &__init_array_start); i++)
    { 
        p[i]();
    }

    main();
}

#ifdef __cplusplus
}
#endif
