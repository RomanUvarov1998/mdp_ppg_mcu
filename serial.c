#include "serial.h"

#define B_T   49
#define Q     50
#define Y     51

static volatile uint8_t response[1];

#define BS 2
static volatile uint8_t uart_buffer[BS];


static void tx_byte(uint8_t data){ 
    while ( !(UCSR0A & (1 << UDRE0)) );
    UDR0 = data;
}

static uint8_t rx_byte(){
    while ( !(UCSR0A & (1 << RXC0)) );
    return UDR0;
}

static void send_buffer(){  
    int i;
    for (i = 0; i < BS; ++i){
        tx_byte(uart_buffer[i]);
    }
}
    
void upload_signal(){
    cli();
            
    init_UART();  
    sendingValueNum = 0;
    stateTx = BEFORE_TX;
    reset_sd_cursor();  
    uint8_t UART_is_busy = 1;
    
    while (UART_is_busy){
        switch (stateTx){
            case BEFORE_TX:
                rx_byte();
                stateTx = SIGNAL_LENGTH_01;
                break;
            case SIGNAL_LENGTH_01: 
                uart_buffer[0] = (uint8_t)(signal_length >> 0);
                uart_buffer[1] = (uint8_t)(signal_length >> 8);
                send_buffer(); 
                stateTx = SIGNAL_LENGTH_23;
                break;
            case SIGNAL_LENGTH_23: 
                uart_buffer[0] = (uint8_t)(signal_length >> 16);
                uart_buffer[1] = (uint8_t)(signal_length >> 24);
                send_buffer(); 
                stateTx = DATA;
                break;
            case DATA:      
                uart_buffer[0] = sd_next_byte();
                uart_buffer[1] = sd_next_byte();
                send_buffer();

                ++sd_cursor.value_num;

                if (!sd_has_next_byte()) {  
                    stateTx = END; 
                    state_set(UPLOADED); 
                    _delay_ms(1000); 
                    UART_is_busy = 0;
                    sei();
                } 
                break;
        }
    }
}

void init_UART(){
    //set baudrate
    UBRR0H = UBRRH_VALUE;//(uint8_t)(ubrr >> 8);
    UBRR0L = UBRRL_VALUE;//(uint8_t)(ubrr & 0xff);
    
    //enable reciever and transmitter
    UCSR0B = ((1 << RXEN0) | (1 << TXEN0));
    
    //set frame format: 8 data, 1 stop bit
    UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00));
}
