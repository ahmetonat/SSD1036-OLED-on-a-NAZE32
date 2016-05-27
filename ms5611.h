
#ifndef MS5611_H
#define MS5611_H

//#define MS5611ADDR 0x77 //Must use 7 bit address, justified left.
#define MS5611ADDR 0xEE

struct ms5611_vars{
  uint32_t ms5611_ut;  // Temp. ADC result
  uint32_t ms5611_up;  // Pressure ADC result
  int32_t pressure;
  int32_t temperature;
  uint16_t CalReg[8];
};

void ms5611_init (struct ms5611_vars * baro); 
void ms5611_measure (struct ms5611_vars * baro); 
void ms5611_calculate (struct ms5611_vars * baro); 

#endif
