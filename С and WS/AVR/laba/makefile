# Имя программы и собранного бинарника
TARGET = main

# путь к каталогу с GCC
AVRCCDIR = C:\\WinAVR-20100110\\bin\\

#само название компилятора, мало ли, вдруг оно когда-нибудь поменяется
CC = avr-gcc
OBJCOPY = avr-objcopy

# каталог в который будет осуществляться сборка, что бы не засерать остальные каталоги
BUILD_DIR = build

# название контроллера для компилятора
MCU = atmega328p

#флаги для компилятора 
OPT = -Os
C_FLAGS = -mmcu=$(MCU) $(OPT) -Wall

# параметры для AVRDUDE
DUDE_MCU = m328p
PORT = COM8
PORTSPEED = 19200
PROGRAMA = avrisp

# DEFINы
DEFINES = \
-D __AVR_ATmega328P__ 
-DF_CPU=1000000UL

# пути к заголовочным файлам
C_INCLUDES =  \
-I C:/WinAVR-20100110/avr/include/ 
#-Iinc

# файлы программы
C_SOURCES = \
src/main.c

# служебные переменные
OBJ_FILES = $(C_SOURCES:.c=.o)
ASM_FILES = $(C_SOURCES:.c=.s)
OUT_OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(OBJ_FILES)))
# правила для сборки

all:
	${CC} ${C_FLAGS} $(C_INCLUDES) -o  $(BUILD_DIR)/${TARGET}.elf $(C_SOURCES)
	${OBJCOPY} -j .text -j .data -O ihex $(BUILD_DIR)/${TARGET}.elf $(BUILD_DIR)/${TARGET}.hex

clean:
	rm -f $(BUILD_DIR)/*


test: 
	avrdude -p $(DUDE_MCU) -c $(PROGRAMA) -P $(PORT) -b $(PORTSPEED) 

prog:
	avrdude -p $(DUDE_MCU) -c $(PROGRAMA) -P $(PORT) -b $(PORTSPEED) -U flash:w:build/main.hex