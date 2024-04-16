#define __AVR_ATmega328P__
#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define serv_1 (249 - 148) / 100 // 1.01
#define serv_2 (249 - 124) / 100 // 1.25
#define serv_3 (255 - 130) / 100 // 1.25

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

void segment(unsigned char G){
    switch(G){
        case 0 : PORTB = 0b01111111;break;
        case 1 : PORTB = 0b00000110;break;
        case 2 : PORTB = 0b10111011;break;
        case 3 : PORTB = 0b10011111;break;
        case 4 : PORTB = 0b11001110;break;
        case 5 : PORTB = 0b11011101;break;
        case 6 : PORTB = 0b11111101;break;
        case 7 : PORTB = 0b00001111;break;
        case 8 : PORTB = 0b11111111;break;
        case 9 : PORTB = 0b11011111;break;
    }
    _delay_ms(10);
}

unsigned char razred1,razred2,razred3,razred4;

void int_chisla(unsigned int chislo){
    razred1 = chislo / 1000;
    razred2 = chislo % 1000 / 100;
    razred3 = chislo % 100 / 10;
    razred4 = chislo % 10;
}

int main(){
    Usart_init();
    sei();
    Shim();
    DDRD |= 0b11001000;
    DDRB |= 0b11111111;
    DDRC |= 0b00001111;
    PORTD |= (1 << PD7);
    PORTC |= 0b00001110;
    ttt_1 = 50;
    ttt_2 = 50;
    OCR2A = ttt_1 + serv_1 + 135;
    OCR2B = ttt_1 + serv_1 + 135;
    int_chisla(50);
    while(1){

        segment(0);
        PORTC = 0b00001110; 
        segment(razred4);
        _delay_ms(1);
        PORTC = 0b00001101;
        segment(razred3);
        _delay_ms(1);
        PORTC = 0b00001011;
        segment(razred2);
        _delay_ms(1);
        PORTC = 0b00000111;
        segment(razred1);
        _delay_ms(1);

        if(RxWords > 0){
            RxWordsLoad(LocBuf);
            if((LocBuf[0] == 'S') && (LocBuf[1] == 'T') && (LocBuf[2] == 'B')){
                mod = 0;
                int_chisla(50);
                ttt_1 = 50;
                ttt_2 = 50;
                PORTD = (1 << PD7) | (0 << PD6);
                OCR2A = ttt_1 + serv_1 + 135;
                OCR2B = ttt_2 + serv_1 + 135;
            }
            if((LocBuf[0] == 'W') && (LocBuf[1] == 'R') && (LocBuf[2] == 'K')){
                mod = 1;
                PORTD = (0 << PD7) | (1 << PD6);
                int_chisla(50);
            }
            if((LocBuf[1] == 'G') && (LocBuf[2] == 'E') && (LocBuf[3] == 'T')){
                switch(LocBuf[0]){
                    case 'R':
                        UsartSendStr("Serva Right set to ");
                        SendInt(ttt_1);
                        UsartPutChar('%');
                        UsartSendStr("\r\n");
                    break;
                    case 'L':
                        UsartSendStr("Serva Left set to ");
                        SendInt(ttt_2);
                        UsartPutChar('%');
                        UsartSendStr("\r\n");
                    break;
                }
            }else if(mod == 1){
                switch(LocBuf[0]){
                    case 'R':
                        ttt_1 = (((LocBuf[1] - '0') * 100) + ((LocBuf[2] - '0') * 10) + (LocBuf[3] - '0'));
                        UsartSendStr("Right\r\n");
                        OCR2A = ttt_1 * serv_2 + 124;
                        int_chisla(ttt_1);
                        _delay_ms(10);
                    break;
                    case 'L':
                        ttt_2 = (((LocBuf[1] - '0') * 100) + ((LocBuf[2] - '0') * 10) + (LocBuf[3] - '0'));
                        UsartSendStr("Left\r\n");
                        OCR2B = ttt_2 * serv_3 + 124;
                        int_chisla(ttt_2);
                        _delay_ms(10);
                    break;
                }
            }
        } 
        _delay_ms(5);
    }
}