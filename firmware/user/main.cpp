#include "device.h"
#include <gpio.h>

#include <adc.h>
#include <i2c.h>
#include <as5600.h>
#include <motor.h>
#include <motor_control.h>

#include <gonio.h>

#include <test.h>
#include <terminal.h>
 
void SetPLL()
{
  RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_12);
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



MotorControl *g_motor_control;
volatile uint32_t g_ms_time = 0;

#ifdef __cplusplus
extern "C" {
#endif

void TIM16_IRQHandler(void)
{ 
  g_motor_control->callback_torque();
  g_ms_time++;
  TIM_ClearITPendingBit(TIM16, TIM_IT_CC1);  
}
 
void TIM17_IRQHandler(void)
{
    g_motor_control->callback(); 

    TIM_ClearITPendingBit(TIM17, TIM_IT_CC1);  
}
 
#ifdef __cplusplus
}
#endif
 
 
void timers_init()
{  
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    NVIC_InitTypeDef            NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);

    TIM_TimeBaseStructure.TIM_Prescaler         = 0;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period            = 24000 - 1; //1kHz, 24MHz clock / 1000 - 1
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0; 
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;   

    TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM16, TIM_IT_CC1, ENABLE);
    TIM_Cmd(TIM16, ENABLE); 

    NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);



    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);

    TIM_TimeBaseStructure.TIM_Prescaler         = 0;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;

    TIM_TimeBaseStructure.TIM_Period            = 48000 - 1; //500Hz, 24MHz clock / 500 - 1
    //TIM_TimeBaseStructure.TIM_Period            = 96000 - 1; //250Hz, 24MHz clock / 250 - 1
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM17, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM17, TIM_IT_CC1, ENABLE);
    TIM_Cmd(TIM17, ENABLE); 

    NVIC_InitStructure.NVIC_IRQChannel = TIM17_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



void motor_identification(Terminal &terminal, MotorControl &motor_control)
{
  motor_control.required_current = MOTOR_CONTROL_MAX; 
  delay_loops(4000000);

  int32_t motor_max_current = 0;
  int32_t motor_max_speed   = 0;

  for (unsigned int i = 0; i < 1000; i++)
  {
    motor_max_current+= motor_control.motor_current;
    motor_max_speed+=   motor_control.angular_velocity;
    delay_loops(1000);
  } 

  motor_max_current = motor_max_current/1000;
  motor_max_speed   = motor_max_speed/1000;


  motor_control.required_current  = 0;
  delay_loops(500000);


  int32_t motor_max_speed_rpm = (motor_max_speed*60)/4096;
  
  terminal << "motor_max_current " << motor_max_current << " mA\n";
  terminal << "motor_max_speed   " << motor_max_speed_rpm   << " rpm\n";
  terminal << "angle             " << motor_control.angle_position   << "\n";

  int32_t time_start = g_ms_time;
  motor_control.required_current = MOTOR_CONTROL_MAX; 

  while (motor_control.angular_velocity < 0.632*motor_max_speed)
  {
    __asm("nop");
  }

  int32_t time_stop = g_ms_time;

  motor_control.required_current  = 0;
  delay_loops(500000);

  int32_t dt = time_stop - time_start; 

  terminal << "time_constant   " << dt   << " ms\n";
  terminal << "\n\n";
}

//const int32_t  required[] = {0, 1024, 0, -1024, 0, 2048, 0, -2048, 0, 4096, 0, -4096, 0, 4*4096, 0, -4*4096};

const int32_t  required[] = {0, 1024, 0, -1024, 0, 5*4096, 0, -5*4096};

 
int main(void)
{
    //cpu clock init
    SystemInit();

    // setup PLL, 6*HSI = 48MHz
    SetPLL();  
  
    Gpio<TGPIOB, 0, GPIO_MODE_OUT> led_0;
    Gpio<TGPIOB, 1, GPIO_MODE_OUT> led_1;

    led_0 = 1;

    
    Terminal terminal;
    terminal.init(115200, USART1);
    terminal << "terminal init done\n";



    
    MotorControl motor_control;
    motor_control.init();

    g_motor_control = &motor_control;
 
    timers_init();
    __enable_irq();

    //motor_identification( terminal, motor_control);

    
    g_ms_time = 0;
    
    while(1)
    {
      if ((g_ms_time%1000) < 50)
      { 
        led_0 = 1; 
      }
      else
      {
        led_0 = 0;
      }

      uint32_t required_idx = (g_ms_time/800)%8;
      
      motor_control.required_position = required[required_idx];

      if ((g_ms_time%100) == 0)
      {
        int32_t angle_deg = (motor_control.angle_position*360)/4096;
        int32_t speed_rpm = (motor_control.angular_velocity*60)/4096;
        terminal << "current " << motor_control.motor_current << " mA\n";
        terminal << "angle " << angle_deg << " deg\n";
        terminal << "speed " << speed_rpm << " rpm\n";
        terminal << "error_sum " << motor_control.error_sum << " deg\n";
        terminal << "\n\n";
      }
    }
    
    /*
    int32_t required[] = {0, 200, 0,  -200};

    while (1) 
    {
      uint32_t idx = (steps/100000)%4;
      
      motor_control.required_current = required[idx];
      
      if (steps%5000 == 0)  
      {
        led_0 = 1;
        terminal << "current = " << motor_control.required_current << " " << motor_control.motor_current << "\n";
        led_0 = 0;
      }  

      steps++;
    }
    */
    
    /*
    uint32_t steps = 0;
    int32_t required[] = {0, 2048, 0,  -2048};
    
    while (1) 
    {
      uint32_t idx = (steps/20000)%4;
      
      motor_control.required_position = required[idx];
      
      if (steps%10000 == 0) 
      {
        terminal << "current = " << motor_control.required_current << " " << motor_control.motor_current << "\n";
        terminal << "angle   = " << (motor_control.required_position*360)/4096 << " " << (motor_control.angle_position*360)/4096 << "\n";
        terminal << "required_current = " << motor_control.required_current << "\n";
        terminal << "\n\n";
      } 

      steps++;
    }
    */
    

    return 0;
} 