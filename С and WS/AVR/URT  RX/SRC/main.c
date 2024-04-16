#define __AVR_ATmega328P__
#define F_CPU 1000000UL     
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define TXsize_buf 255 // рзмер буер
#define RXsize_buf 255 // рзмер буер
////////////////////////////////////////////////////////////////////////////////////
//TX
unsigned txBuffer[TXsize_buf]; // масив равен size_buf
unsigned txbufTail = 0; //хвост
unsigned txbufHead = 0; //голова
unsigned txCount = 0; //наш порядковый номер

////////////////////////////////////////////////////////////////////////////////////
//RX
unsigned RxBuffer[RXsize_buf]; // масив равен size_buf
unsigned RxbufTail = 0; //хвост
unsigned RxbufHead = 0; //голова
unsigned RxCount = 0; //наш порядковый номер

//////////////////////////////////////////////////////////////////////////////////////////////
//TX
void UART_init(){
    UCSR0A |= (1 << U2X0);
    UCSR0B |= (1 << TXEN0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << RXCIE0);
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
    UBRR0 = 12; 
}

void  USART_SendStr(unsigned char *data){
    unsigned char sym; 
        while(*data){
            sym = *data++; 
            USART_PutChar(sym);
        }
}

void USART_PutChar(unsigned char sym){
    if(((UCSR0A & (1 << UDRE0)) != 0) && (txCount == 0)) UDR0 = sym; 
    else{
        if (txCount < TXsize_buf){
            txBuffer[txbufTail] = sym;
            txCount++;
            txbufTail++;
            if (txbufTail == TXsize_buf) txbufTail = 0;
        }        
    }
}

ISR(USART_TX_vect){ 
    if(txCount > 0){
        UDR0 = txBuffer[txbufHead];
        txCount--;
        txbufHead++;
        if(txbufHead == TXsize_buf) txbufHead = 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//RX
unsigned char USART_GetChar(void){//вытаскивает из буфера символ
    unsigned char sym;
    if (RxCount > 0){ //если символов больше чем ноль то читаем и перемещаем голову
        sym = RxBuffer[RxbufHead];
        RxCount--;
        RxbufHead++;
        if (RxbufHead == RXsize_buf) RxbufHead = 0;
        return sym;
    }
}

ISR(USART_RX_vect){//прерывание смотрим и принимаем символы
    if(RxCount < RXsize_buf){
        RxBuffer[RxbufTail] = UDR0;
        RxbufTail++;
        if(RxbufTail == RXsize_buf) RxbufTail = 0;
        RxCount++;
        //USART_SendStr("rerer");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
//серва
void shim1(){ //шим
    TCCR2A |= (1 << WGM20) | (1 << WGM21) | (1 << COM2A1) | (1 << COM2B1); 
    TCCR2B |= (1 << CS21) | (1 << CS20); 
}

/////////////////////////////////////////////////////////////////////////////////////////////
//функция
int al, ah, aa,g;

unsigned char flag = 0;
unsigned char mod = 0;

int main(){
    UART_init();
    sei();
    shim1();
    DDRD = 0b10001000;
    DDRB = 0b00001001;
    while(1){
        if(RxCount > 0){
            _delay_ms(10);
            //if(USART_GetChar() == '1'){}//вытаскиваем символы

            if((USART_GetChar() == 's') && (USART_GetChar() == 't') && (USART_GetChar() == 'o') && (USART_GetChar() == 'p')){
                USART_SendStr("Stop");
                mod = 0;
            }else if(USART_GetChar() == 'w'){
                USART_SendStr("Work");
                mod = 1;
            }

           
            if(mod){

                PORTB = (1 << PB0);
                PORTD = (0 << PD7);

               

                    if((USART_GetChar() == 'L') | (USART_GetChar() == 'R')){
                        
                        if(USART_GetChar() == 'R'){
                         USART_SendStr("Right\n");
                         flag = 0;
                         _delay_ms(10);
                         }else if(USART_GetChar() == 'L'){
                         USART_SendStr("Left\n");
                         flag = 1;
                         _delay_ms(10);
                        }  

                    }else{
                        aa = USART_GetChar() - '0'; //переводим из символа в число
                        al = USART_GetChar() - '0';
                        ah = USART_GetChar() - '0';

                        aa = aa * 100; //домножаем под трёх значное число, писать так 005, 042, 214
                        al = al * 10;
                        g = aa + al + ah;
                    }
                    

                if(flag == 0){
                    OCR2A = g;
                    _delay_ms(10);
                }else if(flag == 1){
                    OCR2B = g;
                    _delay_ms(10);
                }
            }else{
                PORTD = (1 << PD7);
                PORTB = (0 << PB0);
            }

        }
    _delay_ms(100);
    }
        
}