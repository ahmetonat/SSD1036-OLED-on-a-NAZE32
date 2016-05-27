/*******************************************************************/
/*
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
*/
#include "stm32f10x_usart.h"

#include "u8g_arm.h"
#include <u8g.h>
#include <stdio.h>

#include "xprintf.h" // For sprintf and uart printf.
#include "xuart.h"
#include "board_NAZE.h"

#include "i2c.h"    //Check if this is really necessary! AO! Probably not.

static u8g_t u8g;
//"u8g_font_6x10.c"  //A bit too smalll.
//"u8g_font_8x13B.c"  //Looks good.
//"u8g_font_9x15.c" //Easier to read on the SS1036
//"u8g_font_freedoomr10r.c" //Not good.
//"u8g_font_profont17.c" //Not good.
//"u8g_font_courb12r.c"  //Not too great. Courier bold.
//"u8g_font_tpssb.c"  // This is easy to read. 9 pixel high.
//"u8g_font_fub35n.c" // Large and easy to read.
#include "u8g_font_fub11.c"  // Easy to read. But a bit large at 11 pixels. 
#define NORMALFONT u8g_font_fub11

//#include "u8g_font_fub25n.c"


//*******************************************************************
//*******************************************************************
void draw(int words)
{
  u8g_SetFont(&u8g, NORMALFONT);
  
  //u8g_DrawBox(&u8g, 3,41,20,10);// (x,y,width,height)
  //u8g_DrawFrame(&u8g, 25,41,20,10);// (x,y,width,height)
  
  //u8g_DrawCircle(&u8g, 10,58,5,U8G_DRAW_ALL);// (x,y,radius)
  //u8g_DrawDisc(&u8g, 30,58,5,U8G_DRAW_ALL);
  //u8g_DrawDisk(&u8g, x, y, rad, [options])
  
  //u8g_DrawEllipse(&u8g,55,46,8,4,U8G_DRAW_ALL);
  //u8g_DrawFilledEllipse(&u8g,75,46,8,4,U8G_DRAW_ALL);
 
  //u8g_DrawLine(&u8g,84,1,127,16);//( x,y - x,y)
  //u8g_DrawVLine(&u8g,88,38,28);// (x,y,length)
  //u8g_DrawHLine(&u8g,55,55,28);//(x,y,Length)

  //Print a variable: (no float support)
  //a=0;
  //xsprintf(array, "a=%03d",a); 
  //u8g_DrawStr(&u8g, 12, 56, array);

  u8g_DrawRFrame(&u8g, 8,6,96,38,8);// (x,y,h,l,radius)

  if (words==1 || words==3){
    u8g_DrawStr(&u8g, 12, 22, "Merhaba!");
  }

  if (words==2 || words==3){
    u8g_DrawStr(&u8g, 12, 36, "Ali, Zeynep");
   }
  
  //u8g_DrawStr(&u8g, 2, 36, "Sizi Cok Seviyorum");
}

//******************************************************************
int main()
{
 #define BUFSIZE 25
 #define IDLE_WAIT_MS 1000
  
  int i;
  int a;
  char array[BUFSIZE];

  // Turn off JTAG port to be able to use the GPIO for leds
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  //Both below seem to work OK:
  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//Disable only JTAG, keep SWJ on.
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //Disable both JTAG&SWJ

  //"Normal" program continues from here...

  i=0;
  //Elm ChaN's printf over serial port initialization(http://elm-chan.org/):
  // As an extra bonus...
  xdev_out(uart_putc);
  xdev_in(uart_getc);  
  uart_open (USART1,115200,0);
  
  init_LED_GPIO();

  //u8g_InitComFn(&u8g, &u8g_dev_ssd1306_128x64_i2c, u8g_com_hw_i2c_fn);  
  u8g_InitComFn(&u8g, &u8g_dev_ssd1306_128x64_2x_i2c, u8g_com_hw_i2c_fn);  
  // See the bottom of file: u8g_dev_ssd1306_128x64.c

  //The following ritual is a full page render and transfer to display.
  /*  u8g_FirstPage(&u8g);
  do
    { // No draw means "clear screen"      
    } while ( u8g_NextPage(&u8g) );
  */

  LED_OFF();
  a=0;
  
  while(1){
    xprintf("a=%03d\r",a);  // Send the value of a over serial port.
    ++a;
    if (a>3){
      a=1;
    }
    LED_ON(); //LED will show the duty ratio of the screen refresh.
    u8g_FirstPage(&u8g);
    do{
      draw(a);
    } while (u8g_NextPage(&u8g));
    LED_OFF();

    for (i=0;i<100;++i){ //Wait 100ms
      TIM_SetCounter(TIM2, 0);
      while((uint16_t)(TIM2->CNT) <= IDLE_WAIT_MS);
    }
    for (i=0;i<100;++i){ //Wait 100ms
      TIM_SetCounter(TIM2, 0);
      while((uint16_t)(TIM2->CNT) <= IDLE_WAIT_MS);
    }

  } //end while(1)
} //end main()


//******************************************************************
#ifdef USE_FULL_ASSERT
  void assert_failed ( uint8_t* file, uint32_t line)
  {
    /* Infinite loop */
    /* Use GDB to find out why we're here */
    // LED_ON(); //To check...AO!
    while (1);
  }
#endif
