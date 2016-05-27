#ifndef _U8G_ARM_H
#define _U8G_ARM_H

#include <u8g.h>
#include <stm32f10x_conf.h>
//AO!:
//#define CS_ON()   GPIO_SetBits(GPIOA, GPIO_Pin_6)
//#define CS_OFF()  GPIO_ResetBits(GPIOA, GPIO_Pin_6)

//*************************************************************************
void Delay_mss (void);
void init_SPI1(void);
void SPI_Out_LCD12864_Ap (uint8_t Data_LCD12864);
void LCD_OUT (uint8_t Data_LCD, uint8_t Np_LCD);

uint8_t u8g_com_hw_i2c_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr);
#endif


