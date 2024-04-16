# Имя программы и собранного бинарника
PROJECT = TestProg

# путь к каталогу с GCC
AVRCCDIR = C:\\WinAVR-20100110\\bin\\

#само название компилятора, мало ли, вдруг оно когда-нибудь поменяется
CC = avr-gcc
OBJCOPY = avr-objcopy
REMOVE = rm
# название контроллера для компилятора
MCU = atmega328p

#флаги для компилятора 
OPT = -Os
C_FLAGS = -mmcu=$(MCU) $(OPT) -Wall

# параметры для AVRDUDE
DUDE_MCU = atmega2560
PORT = COM6
PORTSPEED = 19200
PROGRAMA = avrisp

# DEFINы
DEFINES = \
-D __AVR_ATmega328P__ 
-DF_CPU=1000000UL

# пути к заголовочным файлам
C_INCLUDES =  \
-I C:/WinAVR-20100110/avr/include/ \
-I ./src/

SRC = $(wildcard ./src/*.c)

all: clean ./build/$(PROJECT).hex

./build/$(PROJECT).elf : $(SRC)
    $(CC) ${C_FLAGS} $(C_INCLUDES) $(SRC) -o ./build/$(PROJECT).elf

./build/$(PROJECT).hex : ./build/$(PROJECT).elf
    ${OBJCOPY} -j .text -j .data -O ihex $< $@

clean:
    $(REMOVE) -f ./build/*

test:
    avrdude -p $(DUDE_MCU) -c $(PROGRAMA) -P $(PORT) -b $(PORTSPEED) -n

prog: ./build/$(PROJECT).hex
    avrdude -p $(DUDE_MCU) -c $(PROGRAMA) -P $(PORT) -b $(PORTSPEED) -U flash:w:./build/$(PROJECT).hex

	
