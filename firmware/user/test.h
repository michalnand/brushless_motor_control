#ifndef _TEST_H_
#define   _TEST_H_

#include <motor.h>
#include <terminal.h>
#include <as5600.h>

void tes_hard_phases_switch();
void test_pwm_waveforms(Motor &motor, int torque);
void test_torque( Motor &motor);
void test_torque_encoder(Motor &motor, AS5600 &encoder);

void encoder_test(Terminal &terminal);

#endif

