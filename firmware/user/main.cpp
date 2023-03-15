#include "device.h"
#include <gpio.h>

#include <i2c.h>
#include <as5600.h>
#include <motor.h>

void SetPLL()
{
  RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_6);
  RCC_PLLCmd(ENABLE);

  // Wait for PLLRDY after enabling PLL.
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET)
  { 
    __asm("nop");
  }

  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.
  SystemCoreClockUpdate();
}


void delay_loops(uint32_t loops)
{
  while (loops--)
  {
    __asm("nop");
  }
}



 
int main(void)
{
  //cpu clock init
  SystemInit();

  // setup PLL, 6*HSI = 48MHz
  SetPLL();  

  Gpio<TGPIOB, 3, GPIO_MODE_OUT> led_pin;

  TI2C<TGPIOB, 7, 6>  i2c;
  AS5600              encoder;

  encoder.init(&i2c);


  Motor motor;


  while (1)
  {
    led_pin = 1;
    delay_loops(1000000);
    
    led_pin = 0;
    delay_loops(1000000);
  }

  return 0;
} 