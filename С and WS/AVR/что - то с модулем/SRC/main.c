#define __AVR_ATmega328P__
#define F_CPU 1000000UL     

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define R_REGISTER 0x00
#define W_REGISTER 0x20
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define NOP 0xFF

#define CONFIG 0x00
 #define MASK_RX_DR 6
 #define MASK_TX_DS 5
 #define MASK_MAX_RT 4
 #define EN_CRC 3
 #define CRCO 2
 #define PWR_UP 1
 #define PRIM_RX 0
#define STATUS 0x07
 #define RX_DR 6
 #define TX_DS 5
 #define MAX_RT 4
 #define RX_P_NO 1
 #define TX_FULL 0
#define RX_PW_P0 0x11




void spi(){
    DDRB |= (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB5);
    PORTB |= (1 << PB2);
    SPCR |= (1 << MSTR) | (1 <<SPE);
}

unsigned char Spi_byte(unsigned char byte){
    SPDR = byte;
    while(!(SPSR &  (1 << SPIF)));
    return SPDR;
}

void nRF_comand(unsigned char comand){
    PORTB |= (0 << PB2);
    Spi_byte(comand);
    PORTB |= (1 << PB2);
}

void nRF_write(unsigned char reg, unsigned char data_out){
    reg |= W_REGISTER;
    PORTB |= (0 << PB2);
    Spi_byte(reg);
    Spi_byte(data_out);
    PORTB |= (1 << PB2);
}

unsigned char nRF_read(unsigned char reg){
    unsigned char data_in;
    PORTB |= (0 << PB2);
    data_in = Spi_byte(reg);
    if(reg != STATUS){
      data_in = Spi_byte(NOP);  
    }
    PORTB |= (1 << PB2);
    return data_in;
}

unsigned char tx_ds_flag, max_rt_flag;

ISR(INT0_vect){
    unsigned char _status = nRF_read(STATUS);
    if(_status & (1 << TX_DS)){
        tx_ds_flag = 1;
    }else if (_status & (1 << MAX_RT)){
        max_rt_flag = 1;
    }
    nRF_write(STATUS, 0x70);
}

int main(){
    spi();
    _delay_ms(100);
    nRF_write(CONFIG, (1 << MASK_RX_DR) | (1 << EN_CRC) | (1 << PWR_UP));
    _delay_ms(2);
    nRF_write(STATUS, 0x70);
    nRF_comand(FLUSH_TX);
    nRF_comand(FLUSH_RX);

    EICRA = (1 << ISC01) | (1 << ISC00);
    EIMSK = (1 << INT0);

    sei();

    while(1){
        nRF_write(W_TX_PAYLOAD, 'D');
        PORTB |= (1 << PB1);
        _delay_ms(11);
        PORTB |= (0 << PB1);
        _delay_ms(135);

        if(tx_ds_flag){
            tx_ds_flag = 0;
        }
        if(max_rt_flag){
            max_rt_flag = 0;
        }
    }
}