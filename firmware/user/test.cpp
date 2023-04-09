#include <test.h>
#include <gonio.h>
#include <gpio.h>
#include <adc.h>

void _delay_loops(uint32_t loops)
{
  while (loops--)
  {
    __asm("nop");
  }
}

void test_hard_phases_switch()
{
  Gpio<TGPIOB, 0, GPIO_MODE_OUT> led_0;
  Gpio<TGPIOB, 1, GPIO_MODE_OUT> led_1;

  led_0 = 1;

  Gpio<TGPIOA, 3, GPIO_MODE_OUT> pa;
  Gpio<TGPIOA, 2, GPIO_MODE_OUT> pb;
  Gpio<TGPIOA, 1, GPIO_MODE_OUT> pc;

  Gpio<TGPIOA, 5, GPIO_MODE_OUT> enable;
  enable = 1;

  uint32_t state = 0;
 
  while(1)
  {
    if (state == 0)
    {
      pa = 0;
      pb = 0;
      pc = 1;
    }
    else if (state == 1)
    {
      pa = 1;
      pb = 0;
      pc = 1;
    }
    else if (state == 2)
    {
      pa = 1;
      pb = 0;
      pc = 0;
    }
    else if (state == 3)
    {
      pa = 1;
      pb = 1;
      pc = 0;
    }
    else if (state == 4)
    {
      pa = 0;
      pb = 1;
      pc = 0;
    }
    else if (state == 5)
    {
      pa = 0;
      pb = 1;
      pc = 1;
    }

    state = (state+1)%6;

    if (state%2 == 0)
    {
      led_1 = 0;
    }
    else
    {
      led_1 = 1;
    }

    _delay_loops(100000);
  }

}

void test_pwm_waveforms(Motor &motor, int torque)
{
    //test PWM waveforms
    
    int32_t a = (SINE_TABLE_SIZE*0)/3;
    int32_t b = (SINE_TABLE_SIZE*1)/3;
    int32_t c = (SINE_TABLE_SIZE*2)/3;

    Gpio<TGPIOB, 0, GPIO_MODE_OUT> led_0;
    Gpio<TGPIOB, 1, GPIO_MODE_OUT> led_1;

    led_0 = 1;

    int speed = 1;

    while (1) 
    {
      int32_t a_phase = (cos_tab(a) + SINE_TABLE_MAX)/2;
      a_phase = (a_phase*MOTOR_CONTROL_MAX*torque)/(SINE_TABLE_MAX*100);

      int32_t b_phase = (cos_tab(b) + SINE_TABLE_MAX)/2;
      b_phase = (b_phase*MOTOR_CONTROL_MAX*torque)/(SINE_TABLE_MAX*100); 
 
      int32_t c_phase = (cos_tab(c) + SINE_TABLE_MAX)/2;
      c_phase = (c_phase*MOTOR_CONTROL_MAX*torque)/(SINE_TABLE_MAX*100);
      
      motor.set_phases(a_phase, b_phase, c_phase);

      a = (a + speed)%SINE_TABLE_SIZE;
      b = (b + speed)%SINE_TABLE_SIZE;
      c = (c + speed)%SINE_TABLE_SIZE;


      if (a%SINE_TABLE_SIZE < SINE_TABLE_SIZE/2)
      {
        led_1 = 0;
      }
      else
      {
        led_1 = 1;
      }

     
      _delay_loops(10);
    }
}

 

void test_torque(Motor &motor)
{
    int32_t   torque        = MOTOR_CONTROL_MAX/1;
    uint32_t  rotor_angle   = 0;

    Gpio<TGPIOB, 0, GPIO_MODE_OUT> led_0;
    Gpio<TGPIOB, 1, GPIO_MODE_OUT> led_1;

    led_0 = 1;

    uint32_t steps = 0;

    //I torque max = 650mA

    while (1)
    {
      motor.set_torque(torque, 0, rotor_angle); 


      rotor_angle = (rotor_angle+1)%4096;

      if (rotor_angle%4096 < 4096/2)
      {
        led_1 = 0;
      }
      else
      {
        led_1 = 1;
      }

      _delay_loops(100);

      steps++;

      
    }
}


void test_torque_encoder(Motor &motor, AS5600 &encoder)
{
    Gpio<TGPIOB, 0, GPIO_MODE_OUT> led_0;
    Gpio<TGPIOB, 1, GPIO_MODE_OUT> led_1;
    Gpio<TGPIOA, 4, GPIO_Mode_AN>  adc_sense;

    led_0 = 1;

    uint32_t steps = 0;

    adc_init();


    int32_t torque = 0;

    //I torque max = 650mA

    while (1)
    {
      //i     = u/r = (adc*3.3/4096)/0.33
      //uref  = 3.3V, R = 0.33ohm, result in mA
      int32_t adc     = adc_read(ADC_Channel_4);
      int32_t current   = (adc*10000)/4096;

    
      encoder.update(1000); 

      //angle ranges from 0..4096
      int32_t angle_mechanical = encoder.read_angle();

      motor.set_torque(torque/10, int32_t(SINE_TABLE_SIZE/4), angle_mechanical); 


      if (angle_mechanical%SINE_TABLE_SIZE < SINE_TABLE_SIZE/2)
      {
        led_1 = 0;
      }
      else
      {
        led_1 = 1;
      }


      torque = (torque + 1)%(MOTOR_CONTROL_MAX*10);

      _delay_loops(100);

      steps++;
    }
}
