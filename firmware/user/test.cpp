#include <test.h>
#include <gonio.h>


void _delay_loops(uint32_t loops)
{
  while (loops--)
  {
    __asm("nop");
  }
}

void test_pwm_waveforms(Motor &motor)
{
    //test PWM waveforms
    
    int32_t a = (SINE_TABLE_SIZE*0)/3;
    int32_t b = (SINE_TABLE_SIZE*1)/3;
    int32_t c = (SINE_TABLE_SIZE*2)/3;

    while (1) 
    {
      int32_t a_phase = (cos_tab(a) + SINE_TABLE_MAX)/2;
      a_phase = (a_phase*MOTOR_CONTROL_MAX)/SINE_TABLE_MAX;

      int32_t b_phase = (cos_tab(b) + SINE_TABLE_MAX)/2;
      b_phase = (b_phase*MOTOR_CONTROL_MAX)/SINE_TABLE_MAX; 
 
      int32_t c_phase = (cos_tab(c) + SINE_TABLE_MAX)/2;
      c_phase = (c_phase*MOTOR_CONTROL_MAX)/SINE_TABLE_MAX;
      
      motor.set_phases(a_phase, b_phase, c_phase);

      a = (a + 1)%SINE_TABLE_SIZE;
      b = (b + 1)%SINE_TABLE_SIZE;
      c = (c + 1)%SINE_TABLE_SIZE;

      _delay_loops(4000);
    }
}

 

void test_torque(Motor &motor)
{
    int32_t   torque        = MOTOR_CONTROL_MAX;
    uint32_t  rotor_angle   = 0;

    while (1)
    {
      motor.set_torque(torque, 0, rotor_angle); 

      rotor_angle = (rotor_angle+1)%SINE_TABLE_SIZE;

      _delay_loops(4000);
    }
}