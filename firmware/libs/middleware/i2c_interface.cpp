#include <i2c_interface.h>



I2C_Interface::I2C_Interface()
{

}

I2C_Interface::~I2C_Interface()
{

}


unsigned char I2C_Interface::check(unsigned char dev_adr)
{
  start();
  unsigned char ack_res = write(dev_adr);
  stop(); 

  return ack_res;
}

void I2C_Interface::write_reg(unsigned char dev_adr, unsigned char reg_adr, unsigned char value)
{
  start();
  write(dev_adr);  //slave address, write command
  write(reg_adr);  //send reg address
  write(value);
  stop();
}

void I2C_Interface::write_reg_extended(unsigned char dev_adr, unsigned int reg_adr, unsigned char value)
{ 
  start();
  write(dev_adr);  //slave address, write command
  write((unsigned char)(reg_adr >> 8));   //send reg address, upper
  write((unsigned char)(reg_adr & 0xFF)); //send reg address, lower
  write(value);
  stop();
}

void I2C_Interface::write_reg_16bit(unsigned char dev_adr, unsigned char reg_adr, unsigned int value)
{
  start();
  write(dev_adr);  //slave address, write command
  write(reg_adr);  //send reg address
  write((value >> 8) & 0xFF);
  write(value  & 0xFF);
  stop();
}

void I2C_Interface::write_reg_multi(unsigned char dev_adr, unsigned char reg_adr, unsigned char *data, unsigned int count)
{
  unsigned int i;
  start();
  write(dev_adr);  //slave address, write command
  write(reg_adr);  //send reg address
  for (i = 0; i < count; i++)
    write(data[i]);
  stop();
}


unsigned char I2C_Interface::read_reg(unsigned char dev_adr, unsigned char reg_adr)
{
  unsigned char res;

  start();
  write(dev_adr);  // slave address, write command
  write(reg_adr);  // send reg address

  start();
  write(dev_adr|0x01); // slave address, read command
  res = read(0);   // read data
  stop();

  return res;
}

unsigned char I2C_Interface::read_reg_extended(unsigned char dev_adr, unsigned int reg_adr)
{
  unsigned char res;

  start();
  write(dev_adr);  // slave address, write command

  write((unsigned char)(reg_adr >> 8));   //send reg address, upper
  write((unsigned char)(reg_adr & 0xFF)); //send reg address, lower

  start();
  write(dev_adr|0x01); // slave address, read command
  res = read(0);   // read data
  stop();

  return res;
}

unsigned int I2C_Interface::read_reg_16bit(unsigned char dev_adr, unsigned char reg_adr)
{
  unsigned int result;

  start();
  write(dev_adr);  // slave address, write command
  write(reg_adr);  // send reg address

  start();
  write(dev_adr|0x01); // slave address, read command
  result = ((unsigned int)read(1))<<8;   // read data
  result|= ((unsigned int)read(0));
  stop();

  return result;
}

void I2C_Interface::read_reg_multi(unsigned char dev_adr, unsigned char reg_adr, unsigned char *data, unsigned int count)
{
  unsigned int i;

  start();
  write(dev_adr);  // slave address, write command
  write(reg_adr);  // send reg address

  start();
  write(dev_adr|0x01); // slave address, read command
  for (i = 0; i < count; i++)
    data[i] = read(1);   // read data
  stop();
}

void I2C_Interface::delay()
{
  for (unsigned int loops = 0; loops < bus_speed_; loops++)
    __asm("nop");
}
