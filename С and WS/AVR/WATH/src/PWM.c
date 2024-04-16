
#define __AVR_ATmega328P__

#include "def.h" //фаил с дефами
#include "PWM.h" //наш заголовочный фаил


void PWM_init(){

DDRB |= 1<<PB3;         //первый сервопривод OCR2A
TCCR2A = (1<<WGM20) | (1<<WGM21) | (1<<COM2A1); 
TCCR2B |= (1 << CS22) | (1<<CS20)  ; 

DDRD |= 1<<PD6;         //второй сервопривод OCR0A
TCCR0A = (1<<WGM00) | (1<<WGM01) | (1<<COM0A1); 
TCCR0B |= (1<<CS00) | (1<<CS02) ; 

} 