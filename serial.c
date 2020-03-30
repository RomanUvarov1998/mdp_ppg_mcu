#include "serial.h"

#define B_T   49
#define Q     50
#define Y     51

void tx_byte(uint8_t data){ 
    while ( !(UCSR0A & (1 << UDRE0)) && next_state == TALK_TO_PC);
    UDR0 = data;
}

uint8_t rx_byte(){
    while ( !(UCSR0A & (1 << RXC0)) && next_state == TALK_TO_PC);
    return UDR0;
}

void UART_on(){
    //set baudrate
    UBRR0H = UBRRH_VALUE;//(uint8_t)(ubrr >> 8);
    UBRR0L = UBRRL_VALUE;//(uint8_t)(ubrr & 0xff);
    
    //enable reciever and transmitter
    UCSR0B = ((1 << RXEN0) | (1 << TXEN0));
    
    //set frame format: 8 data, 1 stop bit
    UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00));
}

void UART_off(){
    //disable reciever and transmitter
    UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));
}

void send_buffer(){
    uint8_t i;
    for (i = 0; i < BUFFER_LENGTH; ++i) {
        tx_byte(buffer[i]);
    }
}
