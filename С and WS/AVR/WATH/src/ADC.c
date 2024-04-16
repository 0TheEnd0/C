
#define __AVR_ATmega328P__

#include <avr/io.h>

#include "def.h" //фаил с дефами
#include "ADC.h" //наш заголовочный фаил

void ADC_init(){

// сам ADC
ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2);

ADMUX = (1 << ADLAR);

} 