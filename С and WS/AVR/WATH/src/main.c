#define __AVR_ATmega328P__     
#define F_CPU 1000000UL 
#include <util/delay.h>
#include <avr/io.h>



unsigned char AL;

#define AD1C 0b00000000
#define AD2C 0b00000001
#define AD3C 0b00000010
#define AD4C 0b00000101

#define void setADC(unsigned char i);



int main(){
    
    ADC_init();
    PWM_init();
    
    while(1){ 
         if((~ADCSRA|~1 << ADSC) == (1 << ADSC)){

             ADMUX = (1 << MUX0) | (1 << MUX1);
             AL = ADCL;
             OCR2A = ADCH;
             _delay_ms(20);

             ADMUX = (1 << MUX2);
             AL = ADCL;
             OCR0A = ADCH;
             _delay_ms(20);

            
             ADCSRA |= (1 << ADSC);
            
         }   

    }
return 0;
}




// void setADC(unsigned char i){
// ADMUX &= ~0b11110000;
// switch (i){
//     case 0 :break;
//     case 1 : ADMUX += AD1C;break;
//     case 2 : ADMUX += AD2C;break;
//     case 3 : ADMUX += AD3C;break;
//     case 4 : ADMUX += AD4C;break;

//     _delay_ms(20);
//     }
// }