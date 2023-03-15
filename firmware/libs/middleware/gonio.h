#ifndef _GONIO_H_
#define _GONIO_H_

#include <stdint.h>

#define SINE_TABLE_SIZE ((unsigned int)1024)
#define SINE_TABLE_MAX  ((int16_t)1024)


//angle in range 0..SINE_TABLE_SIZE-1, unsigned
//SINE_TABLE_MAX corresponds to 1 value
//0 corresponds to -1 value

int16_t sin_tab(uint16_t angle);
int16_t cos_tab(uint16_t angle);

#endif