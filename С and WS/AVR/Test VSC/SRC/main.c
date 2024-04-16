#define __AVR_ATmega328P__
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char units = 0;
unsigned char tens = 0;
unsigned char hundreds = 0;
unsigned char thousand = 0;
unsigned char ds3232Data[19];

void I2C_init(void) {
   TWBR = 0x20;
}

unsigned char I2C_Read(void) {
   TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
   while (!(TWCR & (1 << TWINT)));
   return TWDR;
}

unsigned char I2C_ReadNA(void) {
   TWCR = (1 << TWINT) | (1 << TWEN);
   while (!(TWCR & (1 << TWINT)));
   return TWDR;
}

void I2C_Start(void) {
   TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
   while(!(TWCR & (1 << TWINT)));
}

void I2C_Stop(void) {
   TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void I2C_SendByte(unsigned char c) {
   TWDR = c;
   TWCR = (1 << TWINT) | (1 << TWEN);
   while(!(TWCR & (1 << TWINT)));
}

unsigned char getNumber(unsigned char number) {
   unsigned char ret = 0b00000000;
   
   switch(number) {
      case 0:
   ret = 0b00111111;
   break;
      case 1:
   ret = 0b00000110;
   break;
      case 2:
   ret = 0b01011011;
   break;
      case 3:
   ret = 0b01001111;
   break;
      case 4:
   ret = 0b01100110;
   break;
      case 5:
   ret = 0b01101101;
   break;
      case 6:
   ret = 0b01111101;
   break;
      case 7:
   ret = 0b00000111;
   break;
      case 8:
   ret = 0b01111111;
   break;
      case 9:
   ret = 0b01101111;
   break;
   }
   
   return ret;
}

void showTime() {
   PORTB |= (1 << PB1) | (1 << PB2) | (1 << PB3);
   PORTB &= ~(1 << PB0);
   PORTD = getNumber(thousand);
   
   _delay_ms(10);
   
   PORTB |= (1 << PB0) | (1 << PB2) | (1 << PB3);
   PORTB &= ~(1 << PB1);
   PORTD = getNumber(hundreds);
   
   _delay_ms(10);
   
   PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB3);
   PORTB &= ~(1 << PB2);
   PORTD = getNumber(tens);
   
   _delay_ms(10);
   
   PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
   PORTB &= ~(1 << PB3);
   PORTD = getNumber(units);
   
   _delay_ms(10);
}

void ds3232Init() {
   I2C_Start();
   I2C_SendByte(0b11010000);
   I2C_SendByte(0x0E);
   I2C_SendByte(1<<5);
   I2C_Stop();
}

void getTime() {
   int i;
   I2C_Start();
   I2C_SendByte(0b11010000);
   I2C_SendByte(0b00000000);
   I2C_Stop();
   
   for(i = 0; i < 19; i++) {
      I2C_Start();
      I2C_SendByte(0xD1);
      ds3232Data[i] = I2C_ReadNA();
      I2C_Stop();
   }
   
   units = ds3232Data[0];
   tens = ds3232Data[1];
   hundreds = ds3232Data[2];
   thousand = ds3232Data[3];
}

int main()
 { 
   DDRD = 0b11111111;
   DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2)| (1 << PB3);
   
   I2C_init();
   ds3232Init();
   
   while (1) {
      getTime();
      _delay_ms(50);
      showTime();
   }
   
   return 0;
 }