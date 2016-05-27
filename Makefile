
# compilation flags for gdb

CFLAGS  = -O1 -g
ASFLAGS = -g 

# object files

OBJS  = main.o
OBJS += startup_stm32f10x.o system_stm32f10x.o 
OBJS += stm32f10x_gpio.o stm32f10x_rcc.o
OBJS += stm32f10x_tim.o
OBJS += xprintf.o xuart.o stm32f10x_usart.o
OBJS += i2c.o stm32f10x_i2c.o
#OBJS += ms5611.o
OBJS += board_NAZE.o

#AO! TODO: Remove unnecessary u8b_lib files:
OBJS += u8g_arm.o
OBJS += u8g_dev_ssd1306_128x64.o
OBJS += u8g_font.o u8g_rect.o u8g_circle.o
OBJS += u8g_ellipse.o u8g_line.o
OBJS += u8g_ll_api.o u8g_clip.o u8g_state.o
OBJS += u8g_com_api.o
OBJS += u8g_pb8h1.o u8g_pb32h1.o
OBJS += u8g_pb.o u8g_page.o u8g_com_null.o
OBJS += u8g_pb16v1.o u8g_pb8v1.o
#AO!:
#LDFLAGS_POST = -lm

ELF=$(notdir $(CURDIR)).elf
MAP_FILE=$(notdir $(CURDIR)).map  #AO!
BIN_FILE=$(notdir $(CURDIR)).bin  #AO!

# Tool path
#TOOLROOT=/l/arm/codesourcery/bin AO! Already on the path.

# Library path
LIBROOT=/home/onat/elektronik/ARM/Compiler/STM32F10x_StdPeriph_Lib_V3.5.0

# Tools
CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
OBJCOPY=arm-none-eabi-objcopy  #AO!

#CC=$(TOOLROOT)/arm-none-eabi-gcc
#LD=$(TOOLROOT)/arm-none-eabi-gcc
#AR=$(TOOLROOT)/arm-none-eabi-ar
#AS=$(TOOLROOT)/arm-none-eabi-as

# Code Paths

DEVICE=$(LIBROOT)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CORE=$(LIBROOT)/Libraries/CMSIS/CM3/CoreSupport
PERIPH=$(LIBROOT)/Libraries/STM32F10x_StdPeriph_Driver
#U8GLIB=/home/onat/elektronik/ARM/STM32F100_Book/STM32-Template/SPI_ST792001/F4_st7920_u8glib/u8glib/src/
U8GLIB=/home/onat/elektronik/ARM/Compiler/u8glib_arm1.18.1/src/
U8GFONT=/home/onat/elektronik/ARM/Compiler/u8glib_fonts/

# Search path for standard files
#vpath %.c $(TEMPLATEROOT)

# Search path for perpheral library
vpath %.c $(CORE)
vpath %.c $(PERIPH)/src
vpath %.c $(DEVICE)
vpath %.c $(U8GLIB)

# Search path for Library
#vpath %.c $(TEMPLATEROOT)/Library/ff9/src
#vpath %.c $(TEMPLATEROOT)/Library/ff9/src/option
#vpath %.c $(TEMPLATEROOT)/Library

#  Processor specific
PTYPE = STM32F10X_MD
#PTYPE = STM32F10X_MD_VL
#TODO: Change the processor type. This defines the Proc. clock in the:
#STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/Device
#Support/ST/STM32F10x/system_stm32f10x.c file which is eventually included.

#LDSCRIPT = $(TEMPLATEROOT)/stm32f100.ld
LDSCRIPT = stm32f103.ld

# Compilation Flags
FULLASSERT = -DUSE_FULL_ASSERT 

#LDFLAGS+= -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 
LDFLAGS+= -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -Wl,-Map=$(MAP_FILE) #AO map!
CFLAGS+= -mcpu=cortex-m3 -mthumb 
#CFLAGS+= -I$(TEMPLATEROOT) -I$(DEVICE) -I$(CORE) -I$(PERIPH)/inc -I.
CFLAGS+= -I$(DEVICE) -I$(CORE) -I$(PERIPH)/inc -I.
CFLAGS+= -D$(PTYPE) -DUSE_STDPERIPH_DRIVER $(FULLASSERT)
CFLAGS+= -I$(U8GLIB) -I$(U8GFONT)
#CFLAGS+= -I$(TEMPLATEROOT)/Library/ff9/src -I$(TEMPLATEROOT)/Library -I$(U8GLIB)

OBJCOPYFLAGS = -O binary		   #AO!

# Build executable 

$(BIN_FILE) : $(ELF)				#AO!
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(ELF) : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS) $(LDFLAGS_POST)


# compile and generate dependency info

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

clean:
#	rm -f $(OBJS) $(OBJS:.o=.d) $(ELF) startup_stm32f* $(CLEANOTHER)
	rm -f $(OBJS) $(OBJS:.o=.d) $(ELF) $(MAP_FILE) $(CLEANOTHER) #AO!

debug: $(ELF)
	arm-none-eabi-gdb $(ELF)


# pull in dependencies

-include $(OBJS:.o=.d)




