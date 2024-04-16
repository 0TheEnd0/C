#define __AVR_ATmega328P__
#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define serva_1 (249 - 148) / 100
#define serva_2 (249 - 124) / 100
#define serva_3 (249 - 124) / 100

#define sizeBuff 255

unsigned TxSizBuf[sizeBuff];
unsigned TxHeadbuf = 0;
unsigned TxTailbuf = 0;
unsigned TxCount = 0;

unsigned RxSizeBuf[sizeBuff];
unsigned RxHeadBuf = 0;
unsigned RxTailBuf = 0;
unsigned RxCount = 0;

unsigned char RxWords = 0, LocBuf[], mod, ttt_1,ttt_2;

void Usart_init(){
    UCSR0A |= (1 << U2X0);
    UCSR0B |= (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0) | (1 << TXCIE0);
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
    UBRR0 = 12;
}


void UsartSendStr(unsigned char *data){
    while(*data){
        UsartPutChar(*data++);
    }
}

void UsartPutChar(unsigned char sym){
    if(((UCSR0A & (1 << UDRE0)) != 0) && (TxCount == 0)){
        UDR0 = sym;
    }else{
        if(TxTailbuf < sizeBuff){
            TxSizBuf[TxTailbuf] = sym;
            TxTailbuf++;
            TxCount++;
            if(TxTailbuf == sizeBuff){
            TxTailbuf == 0;
            }
        }
    }
}

ISR(USART_TX_vect){
    if(TxCount > 0){
        UDR0 = TxSizBuf[TxHeadbuf];
        TxHeadbuf++;
        TxCount--;
        if(TxHeadbuf == sizeBuff){
            TxHeadbuf = 0;
        }
    }
}

unsigned char GetChar(void){
    unsigned char sym;
    if(RxCount > 0){
        sym = RxSizeBuf[RxHeadBuf];
        RxHeadBuf++;
        RxCount--;
        if(RxHeadBuf == RxSizeBuf){
            RxHeadBuf = 0;
        }
        return sym;
    }
}

ISR(USART_RX_vect){
    if(RxTailBuf < sizeBuff){
        RxSizeBuf[RxTailBuf] = UDR0;
        if(RxSizeBuf[RxTailBuf] == 13){
            RxWords++;
        }
        RxTailBuf++;
        RxCount++;
        if(RxTailBuf == sizeBuff){
            RxTailBuf = 0;
        }
    }
}

void RxWordsLoad(unsigned char *data){
    unsigned char dat;
    while(dat != 13){
        dat = GetChar();
        *data = dat;
        data++;
    }
    RxWords--;
}

void Shim(){
    TCCR2A |= (1 << WGM20) | (1 << WGM21) | (1 << COM2A1) | (1 << COM2B1);
    TCCR2B |= (1 << CS21);
}

void SendInt(unsigned char F){
    unsigned char A;
    A = F / 100;
    UsartPutChar(A % 10 + '0');
    A = F % 100;
    UsartPutChar(A / 10 + '0');
    UsartPutChar(A % 10 + '0');
}

int main(){
    Usart_init();
    sei();
    Shim();
    DDRD |= 0b11001000;
    DDRB |= 0b00001000;
    PORTD |= (1 << PD7);
    ttt_1 = 50;
    ttt_2 = 50;
    OCR2A = ttt_1 + serva_1 + 136;
    OCR2B = ttt_1 + serva_1 + 136;
    while(1){
        if(RxWords > 0){
            RxWordsLoad(LocBuf);
            if((LocBuf[0] == 'S') && (LocBuf[1] == 'T') && (LocBuf[2] == 'B')){
                mod = 0;
                ttt_1 = 50;
                ttt_2 = 50;
                PORTD |= (1 << PD7) | (0 << PD6);
                OCR2A = ttt_1 + serva_1 + 136;
                OCR2B = ttt_2 + serva_1 + 136;
            }
            if((LocBuf[0] == 'W') && (LocBuf[1] == 'R') && (LocBuf[2] == 'K')){
                mod = 1;
                PORTD |= (0 << PD7) | (1 << PD6);
            }
            if((LocBuf[1] == 'G') && (LocBuf[2] == 'E') && (LocBuf[3] == 'T')){
                switch(LocBuf[0]){
                    case 'R' : 
                    UsartSendStr(" Right Serva set to ");
                    SendInt(ttt_1);
                    UsartPutChar('%');
                    UsartSendStr("\r\n");
                    break;

                    case 'L' : 
                    UsartSendStr(" Left Serva set to ");
                    SendInt(ttt_2);
                    UsartPutChar('%');
                    UsartSendStr("\r\n");
                    break;
                }
            }else if(mod == 1){
                switch(LocBuf[0]){
                    case 'R' :
                    ttt_1 = (((LocBuf[1] - '0') * 100) && ((LocBuf[2] - '0') * 10) && (LocBuf[3] - '0'));
                    UsartSendStr("Right\r\n");
                    OCR2A = ttt_1 * serva_2 + 124;
                    _delay_ms(20);
                    break;

                    case 'L' :
                    ttt_2 = (((LocBuf[1] - '0') * 100) && ((LocBuf[2] - '0') * 10) && (LocBuf[3] - '0'));
                    UsartSendStr("Left\r\n");
                    OCR2B = ttt_1 * serva_2 + 124;
                    _delay_ms(20);
                    break;
                }
            }
        } 
        _delay_ms(200);
    }
}