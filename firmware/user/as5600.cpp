#include "as5600.h"


#define I2C_ADDRESS ((unsigned char)0x36<<1)

#define ZMCO_ADR    ((unsigned char)0x00)
#define ZPOS_H_ADR  ((unsigned char)0x01)
#define ZPOS_L_ADR  ((unsigned char)0x02)
#define MPOS_H_ADR  ((unsigned char)0x03)
#define MPOS_L_ADR  ((unsigned char)0x04)
#define MANG_H_ADR  ((unsigned char)0x05)
#define MANG_L_ADR  ((unsigned char)0x06)
#define CONF_H_ADR  ((unsigned char)0x07)
#define CONF_L_ADR  ((unsigned char)0x08)

#define RAW_ANGLE_H_ADR  ((unsigned char)0x0C)
#define RAW_ANGLE_L_ADR  ((unsigned char)0x0D)
#define ANGLE_ANGLE_H_ADR  ((unsigned char)0x0E)
#define ANGLE_ANGLE_L_ADR  ((unsigned char)0x0F)

#define STATUS_ADR  ((unsigned char)0x0B)
#define AGC_ADR     ((unsigned char)0x1A)
#define MAGNITUDE_H_ADR     ((unsigned char)0x1B)
#define MAGNITUDE_L_ADR     ((unsigned char)0x1C)


AS5600::AS5600()
{

}

AS5600::~AS5600()
{

}

int AS5600::init(I2C_Interface *i2c_)
{
    this->i2c           = i2c_;
    this->angle         = 0;
    this->zero_angle    = 0;

    //power on
    //hysteresis off
    //slow filter only
    i2c->write_reg(I2C_ADDRESS, CONF_L_ADR, 0x00); 
    i2c->write_reg(I2C_ADDRESS, CONF_H_ADR, (1<<0)|(1<<1)); 

    this->read();
    this->set_zero();
    
    return 0;
}

int32_t AS5600::read()
{
    int32_t angle;
    angle = (uint16_t)i2c->read_reg(I2C_ADDRESS, RAW_ANGLE_H_ADR) << 8;
    angle|= (uint16_t)i2c->read_reg(I2C_ADDRESS, RAW_ANGLE_L_ADR);

    this->angle = angle - this->zero_angle;

    return this->angle;
}

void AS5600::set_zero()
{
    this->zero_angle = this->angle;
}