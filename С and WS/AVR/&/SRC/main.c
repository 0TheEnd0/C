
#define __AVR_ATmega328P__
#define F_CPU 1000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define serv_1 (249 - 148) / 100 // 1.01
#define serv_2 (249 - 124) / 100 // 1.25
#define serv_3 (249 - 124) / 100 // 1.25
#define sizeBuf 36

unsigned TxBuf[sizeBuf];
unsigned TxBufHead = 0;
unsigned TxBufTail = 0;
unsigned TxCount = 0;

unsigned RxBuf[sizeBuf];
unsigned RxBufHead = 0;
unsigned RxBufTail = 0;
unsigned RxCount = 0;

unsigned char LocBuf[], mod, ttt_1, ttt_2,RxWordCount;

void USART_init(){
    UCSR0A |= (1 << U2X0);
    UCSR0B |= (1 << TXEN0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << RXCIE0);
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
    UBRR0 = 12;
}

void USARTSendStr(unsigned char *data){
    while(*data){
        USARTPutChar(*data++);
    }
}

void USARTPutChar(unsigned char sym){
    if(((UCSR0A & (1 << UDRE0)) != 0) && (TxCount == 0)){
        UDR0 = sym;
    }else{
        if(TxCount < sizeBuf){
            TxBuf[TxBufTail] = sym;
            TxCount++;
            TxBufTail++;
            if(TxBufTail == sizeBuf){
                TxBufTail = 0;
            }
        }
    }
}

ISR(USART_TX_vect){
    if(TxCount > 0){
        UDR0 = TxBuf[TxBufHead];
        TxCount--;
        TxBufHead++;
        if(TxBufHead == sizeBuf){
            TxBufHead = 0;
        }
    }
}

unsigned char USARTGetChar(void){
    unsigned char sym;
    if(RxCount > 0){
        sym = RxBuf[RxBufHead];
        RxCount--;
        RxBufHead++;
        if(RxBufHead == sizeBuf){
            RxBufHead = 0;
        }
        return sym;
    }
}

ISR(USART_RX_vect){
    if(RxCount < sizeBuf){
        RxBuf[RxBufTail] = UDR0;
        if(RxBuf[RxBufTail] == 13){RxWordCount++;}
        RxCount++;
        RxBufTail++;
        if(RxBufTail == sizeBuf){
            RxBufTail = 0;
        }
    }
}

void shim(){
    TCCR2A |= (1 << WGM20) | (1 << WGM21) | (1 << COM2A1) | (1 << COM2B1);
    TCCR2B |= (1 << CS21);
}

void RxLoadWord(unsigned char *data){
    unsigned char dat;
    while(dat != 13){
        dat = USARTGetChar();
        *data = dat;
        data++;
    }
    RxWordCount--;
}

void SendInt(unsigned char F){
    unsigned char A;
    A = F / 100;
    USARTPutChar(A % 10 + '0');
    A = F % 100;
    USARTPutChar(A / 10 + '0');
    USARTPutChar(A % 10 + '0');
}

int main(){
    USART_init();
    sei();
    shim();
    DDRD |= 0b11001000;
    DDRB |= 0b00001000;
    PORTD = (1 << PD7);
    ttt_1 = 50;
    ttt_2 = 50;
    OCR2A = ttt_1 * serv_1 + 136;
    OCR2B = ttt_2 * serv_1 + 136; 
    while(1){
        if(RxWordCount > 0){
            RxLoadWord(LocBuf);
            if((LocBuf[0] == 'S') & (LocBuf[1] == 'T') & (LocBuf[2] == 'B')){
                mod = 0;
                PORTD = 0b10001000;
                ttt_1 = 50;
                ttt_2 = 50;
                OCR2A = ttt_1 * serv_1 + 136;
                OCR2B = ttt_2 * serv_1 + 136;
            }
            if((LocBuf[0] == 'W') & (LocBuf[1] == 'R') & (LocBuf[2] == 'K')){
                mod = 1;
                PORTD = 0b01001000;
            }
            if((LocBuf[1] == 'G') && (LocBuf[2] == 'E') && (LocBuf[3] == 'T')){
                switch(LocBuf[0]){
                    case 'R':
                        USARTSendStr("Serva Right set to ");
                        SendInt(ttt_1);
                        USARTPutChar('%');
                        USARTSendStr("\r\n");
                    break;
                    case 'L':
                        USARTSendStr("Serva Left set to ");
                        SendInt(ttt_2);
                        USARTPutChar('%');
                        USARTSendStr("\r\n");
                    break;
                }
            }else if(mod == 1){
                switch(LocBuf[0]){
                    case 'R':
                        ttt_1 = (((LocBuf[1] - '0') * 100) + ((LocBuf[2] - '0') * 10) + (LocBuf[3] - '0'));
                        USARTSendStr("Right\r\n");
                        OCR2A = ttt_1 * serv_2 + 124;
                        _delay_ms(20);
                    break;
                    case 'L':
                        ttt_2 = (((LocBuf[1] - '0') * 100) + ((LocBuf[2] - '0') * 10) + (LocBuf[3] - '0'));
                        USARTSendStr("Left\r\n");
                        OCR2B = ttt_2 * serv_3 + 124;
                        _delay_ms(20);
                    break;
                }
            }
        }
        _delay_ms(500);
    }
}