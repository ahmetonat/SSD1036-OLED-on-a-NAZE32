
#include "board_NAZE.h"


void LED_ON(void){
  GPIO_WriteBit(LED_PORT,LED_PIN,Bit_RESET);
  }

void LED_OFF(void){
  GPIO_WriteBit(LED_PORT,LED_PIN,Bit_SET);
    }

void init_LED_GPIO (void){

  GPIO_InitTypeDef GPIO_InitStruct;

  // Enable GPIOB (LED) Clock:
  RCC_APB2PeriphClockCmd ( LED_PORT_RCC, ENABLE ); 

  //Initilize the structure values to something sane:
  GPIO_StructInit (&GPIO_InitStruct); 
                                      
  //Initialize LED port:
  GPIO_InitStruct.GPIO_Pin = LED_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED_PORT, &GPIO_InitStruct);

  //  LED_ON();
}
