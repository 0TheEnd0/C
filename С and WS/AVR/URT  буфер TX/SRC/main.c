#define __AVR_ATmega328P__
#define F_CPU 1000000UL     
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define size_buf 110 // рзмер буер

unsigned TxBuffer[size_buf]; // масив равен size_buf
unsigned txbufTail = 0; //хвост
unsigned txbufHead = 0; //голова
unsigned txCount = 0; //наш порядковый номер



// void TxData(unsigned char BUKVA){
//     while(!(UCSR0A & (1 << UDRE0)));
//     UDR0 = BUKVA;
// }

void UART_init(){
    UCSR0A |= (1 << U2X0);//Удвоение скорости
    UCSR0B |= (1 << TXEN0) | (1 << TXCIE0);//Включаем передатчик и прерывание
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);//Размер пакет 8 бит   
    UBRR0 = 12; //Скорость 9600 
}

void  USART_SendStr(unsigned char *data){//отправляем и разбиваем строку на символы *data указатель
    unsigned char sym; //переменная для записи символов
        while(*data){//условие при котором мы заносим символы пока они не отрицательные
            sym = *data++; 
            USART_PutChar(sym);//закидываем символ
        }
}

void USART_PutChar(unsigned char sym){
    if(((UCSR0A & (1 << UDRE0)) != 0) && (txCount == 0)) UDR0 = sym; //условие при котором мы проверяем есть ли место в буфере и свободен ли юдр, если он свободен то закидываем символ 
    else{//если юдр занят то заносим в буфер
        if (txCount < size_buf){//нужное условие для того чтобы номер был меньше числа
            TxBuffer[txbufTail] = sym;//заносим символ в хвост/конец
            txCount++;//+1 к номеру
            txbufTail++;//+1 к хвосту
            if (txbufTail == size_buf) txbufTail = 0;//если хвост равен буферу то он достиг начала
        }        
    }
}

ISR(USART_TX_vect){ //прерывание по вектеру тх, 
    if(txCount > 0){
        UDR0 = TxBuffer[txbufHead]; //заносим новый символ в юдр
        txCount--; //освобождаем ячейку ибо прочитали
        txbufHead++; //тк прочитали ячейку идём к следуйщей
        if(txbufHead == size_buf) txbufHead = 0; //если голова будет равна началу то начался новый цикл круга
    }
}

// void ADC_init(){
//     ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADPS2);
//     ADMUX |= (1 << MUX2); 
// }

// void send_int_Uart(int c){ //функция для разделение числа в символ и его отправка
//     int temp;
//     temp = c / 100;
//     TxData(temp / 10 + '0');
//     TxData(temp % 10 + '0');
//     temp = c % 100;
//     TxData(temp / 10 + '0');
//     TxData(temp % 10 + '0');
// }

int main(){
    UART_init();
    sei(); //разрешает прерывание
    while(1){
      USART_SendStr("CCCP SLAVE"); //наши символы
      _delay_ms(20);
    }
    return 0;
}