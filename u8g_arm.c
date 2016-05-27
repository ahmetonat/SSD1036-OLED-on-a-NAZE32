

//Specialized for I2C SSD1306 OLED display of 128x64, monochrome.

//See: http://blog.bastelhalde.de/?p=759
// Also see: http://elastic-notes.blogspot.com.tr/p/stm32-i2c-oled-ssd.html
// Google search: stm32f SSD1306 i2c u8glib

#include "u8g_arm.h"
#include "i2c.h"

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_i2c.h>
#include <stm32f10x_tim.h>

#define DELAY_TIM_FREQUENCY 1000000

#define DATA_BUFFER_SIZE 120   //This worked.
//#define DATA_BUFFER_SIZE 500
//#define DATA_BUFFER_SIZE 1025
//#define DATA_BUFFER_SIZE 2050
#define SSD1306_ADDR 0x78      //Full 8 bit address, justified left.

/*void I2C_StartTransmission(I2C_TypeDef* I2Cx, uint8_t transmissionDirection,uint8_t slaveAddress);
void I2C_WriteData(I2C_TypeDef* I2Cx, uint8_t data);
*/

uint8_t control = 0;

/* set TIM2 to run at DELAY_TIM_FREQUENCY */
void delay_init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;


  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Enable TIM2 clock 
 
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);  // Time base configuration 

  TIM_TimeBaseStructure.TIM_Period = 65535;
  //TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/DELAY_TIM_FREQUENCY)-1;
  TIM_TimeBaseStructure.TIM_Prescaler=0; //From examples...
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  //TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //Do not divide.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_PrescalerConfig(TIM2,(uint16_t) (SystemCoreClock/DELAY_TIM_FREQUENCY)-1, TIM_PSCReloadMode_Immediate);   // Prescaler configuration

  TIM_Cmd(TIM2, ENABLE);   // Enable counter 
}

/*  Delay by the provided number of micro seconds.
    Limitation: "us" * System-Freq in MHz must not overflow in 16 bit.
    Values between 0 and 65535 us are ok.
*/
void delay_micro_seconds(uint16_t us) {
  TIM_SetCounter(TIM2, 0);
  while((uint16_t)(TIM2->CNT) <= us);
}

/*========================================================================*/
/*
  The following delay procedures must be implemented for u8glib

  void u8g_Delay(uint16_t val)	Delay by "val" milliseconds
  void u8g_MicroDelay(void)	Delay be one microsecond
  void u8g_10MicroDelay(void)	Delay by 10 microseconds
*/

void u8g_Delay(uint16_t val){
  delay_micro_seconds(1000UL*(uint32_t)val);
}

void u8g_MicroDelay(void){
  delay_micro_seconds(1);
}

void u8g_10MicroDelay(void){
  delay_micro_seconds(10);
}


//*************************************************************************
uint8_t u8g_com_hw_i2c_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr) { 
  uint8_t buffer2[2];
  uint8_t bufferDISP[DATA_BUFFER_SIZE];

  switch(msg){
  case U8G_COM_MSG_STOP:
    //STOP THE DEVICE
    break;
    
  case U8G_COM_MSG_INIT:
    //INIT HARDWARE INTERFACES, TIMERS, GPIOS...		
    //void I2C_LowLevel_Init(I2C_TypeDef* I2Cx, int ClockSpeed, int OwnAddress);
    I2C_LowLevel_Init(I2C2, 200000, 0);     //Works OK.
    delay_init();
    u8g_MicroDelay(); //By the book...
    break;
    
  case U8G_COM_MSG_ADDRESS:  
    // define cmd mode (arg_val = 0) or data mode (arg_val = 1) 
    if(arg_val == 0){
      control = 0;
    }else{
      control = 0x40;  //ssd1306 specs...
    }
    u8g_10MicroDelay(); //By the book...
    break;
    
  case U8G_COM_MSG_RESET://RESET PIN BY THE VALUE IN arg_val
    // There is no reset pin on this display... Do nothing.
    break;
    
  case U8G_COM_MSG_WRITE_BYTE:    //WRITE BYTE TO DEVICE

    //This worked OK:    
    buffer2[0]=control;
    buffer2[1]=arg_val;
    I2C_Write(I2C2, buffer2, 2,  SSD1306_ADDR);

    // The line below did not work:
    //I2C_Write(I2C2, arg_val, 1,  SSD1306_ADDR); //AO! This does not work.

    /* AO! From a simpler implementation. Did not work.
    I2C_StartTransmission (I2C2, I2C_Direction_Transmitter, SSD1306_ADDR);
    I2C_WriteData(I2C2, control);
    I2C_WriteData(I2C2, arg_val);	
    I2C_GenerateSTOP(I2C2, ENABLE); 
    */

    u8g_MicroDelay();		
    break;
    
  case U8G_COM_MSG_WRITE_SEQ:
  case U8G_COM_MSG_WRITE_SEQ_P:    //WRITE A SEQUENCE OF BYTES TO THE DEVICE
    {
      int i;
      register uint8_t *ptr = arg_ptr;
      bufferDISP[0]=control;
      for (i=1; i<=arg_val;i++){
	bufferDISP[i]=*(ptr++);
      }
      I2C_Write(I2C2, bufferDISP, arg_val+1,  SSD1306_ADDR);
      // See the second reference at the top.
    }		
    break;
  }
  return 1;
}



