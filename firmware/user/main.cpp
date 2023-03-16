#include "device.h"
#include <gpio.h>

#include <i2c.h>
#include <as5600.h>
#include <motor.h>

#include <gonio.h>

#include <test.h>
#include <terminal.h>

void SetPLL()
{
  RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_6);
  RCC_PLLCmd(ENABLE);

  // Wait for PLLRDY after enabling PLL.
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET)
  { 
    __asm("nop");
  } 

  // Select the PLL as clock source.
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  
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

    /*
    Terminal terminal;
    terminal.init(115200, USART2);
    terminal << "terminal init done\n";
  
    TI2C<TGPIOB, 7, 6>  i2c;
    AS5600              encoder;
    encoder.init(&i2c);

    
    terminal << "device init done\n";

    while(1)
    {
      int32_t angle = encoder.read();

      terminal << "angle = " << angle << "\n";

      delay_loops(100000);
    }
    */

  
    Motor motor;
    motor.init();

    //test_pwm_waveforms(motor);
    test_torque(motor);
    


    return 0;
} 