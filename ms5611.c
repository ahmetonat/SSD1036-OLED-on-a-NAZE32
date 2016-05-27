
#include <stm32f10x.h>
#include "i2c.h"
#include "ms5611.h"

extern void Delay( uint32_t nTime );

    //MS5611 Max conversion time 9.04ms for 4096 oversampling ratio.
    //After the conversion, using ADC read command the result is
    //clocked out with the MSB first. If the conversion is not executed
    //before the ADC read command, or the ADC read command is repeated,
    //it will give 0 as the output result. If the ADC read command is sent
    //during conversion the result will be 0, the conversion will
    //not stop and the final result will be wrong. Conversion sequence
    //sent during the already started conversion process will yield
    //incorrect result as well.

void ms5611_init (struct ms5611_vars * baro){
  uint8_t I2Cregister[3];
  int i;
  //I2C_Write(I2C_TypeDef*I2Cx, const uint8_t*buf, uint32_t nbyte, uint8_t SlaveAddress)
  const uint8_t command[12] = {0x00,0x48,0x58,0x1e,0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae};
  //readADC,convD1 press,convD2 temp,reset,CalReg0,...,CalReg7

  I2C_Write (I2C1, command+3, 1, MS5611ADDR); //Reset MS5611
  //Reset sequence takes 2.8ms.
  Delay(3);
  
  for (i=0;i<8;++i){
    I2C_Write (I2C1, command+4+i, 1, MS5611ADDR); //Set register to be read
    I2C_Read (I2C1, I2Cregister, 2, MS5611ADDR);
    baro->CalReg[i]= ((I2Cregister[0]<<8) | I2Cregister[1]);
  }
  //  for (i=0;i<8;++i){
  //    xprintf ("Cal.Reg. %d: %d\n\r",i,CalReg[i]);
  //  }
}

void ms5611_measure(struct ms5611_vars * baro){
  uint8_t I2Cregister[3];
  const uint8_t command[12] = {0x00,0x48,0x58,0x1e,0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae};

  //read D1 and D2 from pressure sensor:
  I2C_Write (I2C1, command+1, 1, MS5611ADDR); //Start ADC for pressure.
  Delay(10);                                  //Blocking wait for ADC end.
  I2C_Write (I2C1, command, 1, MS5611ADDR);   //Command: Read ADC
  I2C_Read (I2C1, I2Cregister, 3, MS5611ADDR);
  baro->ms5611_up= ((I2Cregister[0]<<16) | I2Cregister[1]<<8 | I2Cregister[2]);

  I2C_Write (I2C1, command+2, 1, MS5611ADDR); //Start ADC for temperature.
  Delay(10);                                  //Blocking wait for ADC end.
  I2C_Write (I2C1, command, 1, MS5611ADDR);   //Command: Read ADC
  I2C_Read (I2C1, I2Cregister, 3, MS5611ADDR);
  baro->ms5611_ut= ((I2Cregister[0]<<16) | I2Cregister[1]<<8 | I2Cregister[2]);
  //  xprintf("Temp raw: %d, press raw:%d\n\r",ms5611_ut, ms5611_up);

}

void ms5611_calculate(struct ms5611_vars * baro){
  uint32_t press;
  int64_t temp;
  int64_t delt;
  int32_t dT = (int64_t)baro->ms5611_ut - ((uint64_t)baro->CalReg[5] * 256);
  int64_t off=((int64_t)baro->CalReg[2]<<16)+(((int64_t)baro->CalReg[4]*dT)>>7);
  int64_t sens=((int64_t)baro->CalReg[1]<<15)+(((int64_t)baro->CalReg[3]*dT)>>8);
  temp = 2000 + ((dT * (int64_t)baro->CalReg[6]) >> 23);
  
  if (temp < 2000) { // temperature lower than 20degC
    delt = temp - 2000;
    delt = 5 * delt * delt;
    off -= delt >> 1;
    sens -= delt >> 2;
    if (temp < -1500) { // temperature lower than -15degC
      delt = temp + 1500;       //TODO: Kontrol ediecek.
      delt = delt * delt;
      off -= 7 * delt;          // Bunun += olmasi lazim?
      sens -= (11 * delt) >> 1; // Bunun += olmasi lazim?
    }
  }
  press = ((((int64_t)baro->ms5611_up * sens) >> 21) - off) >> 15;

  baro->pressure = press;
  baro->temperature = temp;
}

