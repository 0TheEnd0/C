#define __AVR_ATmega328P__
#define F_CPU 1000000UL     
#include <util/delay.h>
#include <avr/io.h>



void TxData(unsigned char BUKVA){
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = BUKVA;
}

void UART_init(){
    UCSR0A |= (1 << U2X0);//Удвоение скорости
    UCSR0B |= (1 << TXEN0);//Включаем передатчик
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);//Размер пакет 8 бит   
    UBRR0 = 12; //Скорость 9600 
}

void ADC_init(){
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADPS2);
    ADMUX |= (1 << MUX2); 
}

void send_int_Uart(int c){
    int temp;
    temp = c / 100;
    TxData(temp / 10 + '0');
    TxData(temp % 10 + '0');
    temp = c % 100;
    TxData(temp / 10 + '0');
    TxData(temp % 10 + '0');
}

int g;

int main(){
    UART_init();
    ADC_init();
    while(1){
        if(((ADCSRA)&(1 << ADSC)) == 0){
            g = ADC;
            send_int_Uart(g);
            TxData('C');
            _delay_ms(3);
            ADCSRA |= (1 << ADSC); 
            _delay_ms(100);
        }
    }
    return 0;
}