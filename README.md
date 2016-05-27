# SSD1036-OLED-on-a-NAZE32

STM32F103 is a very nice microcontroller; quite powerful, fast, easy to use, and there are a lot of examples on various applications. This project shows how a popular SSD1306 OLED displays can be connected to STM32F103, over I2C port 2, and using u8glib, by olikarus, at:
https://github.com/olikraus/u8glib

There are many cheap development boards for the STM32F103 on the Internet, but another source of good development boards is to repurpose existing boards. I used the Naze32 (or one of its clones to be exact, the Flip 32), which come at a price of about US$20. It was designed for quadcopters and thus has many sensors integrated, voltage regulator clock etc. It is a good bargain. The SSD1306 OLED module is already running on Naze32, thanks to CleanFlight control program, and theoretically if you want to write your own, you can carefully pick out parts from CleanFlight. Unfortunately CleanFlight is quite large, and it was made for many processors, so it is easy to get lost in the source. So I decided to write my own.

The graphics library is u8glib by olikarus. it is hugely popular, but with one significant drawback: It was written for Arduino. There is an ARM port, but it requires you to write a few extensions for your particular processor, namely three delay functions, initialization of the connection peripheral, and write to the peripheral. These have been implemented in the files u8g_arm.c and u8g_arm.h.

