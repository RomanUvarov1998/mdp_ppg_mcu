#include "serial.h"

#define BS 2
#define SL 500

volatile uint32_t signalLength;
volatile uint16_t signalValues[SL];
volatile uint32_t sendingValueNum;
volatile uint8_t writeBuffer[BS];

#define B_T   49
#define Q     50
#define Y     51
uint8_t response[1];

volatile uint32_t signalLength = SL;
volatile uint32_t sendingValueNum = 0;

enum StatesTx {
    BEFORE_TX = 0,
    SIGNAL_LENGTH_01 = 1,
    SIGNAL_LENGTH_23 = 2,
    DATA = 3,
    END = 4,
};

volatile uint8_t stateTx = BEFORE_TX;
    
void prepareToUploading(){    
    sendingValueNum = 0;
    stateTx = BEFORE_TX;
}

void stateProcessing(){
    switch (stateTx){
        case BEFORE_TX:
            /////////////////
            response[0] = receiveByte();
            if (response[0] == B_T) {
                stateTx = SIGNAL_LENGTH_01;
            }
            break;
            ///////////////////////
            break;
        case SIGNAL_LENGTH_01: 
            writeBuffer[0] = (uint8_t)(signalLength >> 0);
            writeBuffer[1] = (uint8_t)(signalLength >> 8);
            SendBuffer(); 
            stateTx = SIGNAL_LENGTH_23;
            break;
        case SIGNAL_LENGTH_23: 
            writeBuffer[0] = (uint8_t)(signalLength >> 16);
            writeBuffer[1] = (uint8_t)(signalLength >> 24);
            SendBuffer(); 
            stateTx = DATA;
            break;
        case DATA:      
            writeBuffer[0] = (uint8_t)(signalValues[sendingValueNum] >> 0);
            writeBuffer[1] = (uint8_t)(signalValues[sendingValueNum] >> 8);
            SendBuffer();
            ++sendingValueNum;
            if (sendingValueNum >= signalLength) {
                stateTx = END; 
                state_set(UPLOADED);
                _delay_ms(1000); 
            }
            break;
    }
}

void createSignal(){
    int i;
    for (i = 0; i < signalLength; ++i) signalValues[i] = i;
}

void initUART(){
    //set baudrate
    UBRR0H = UBRRH_VALUE;//(uint8_t)(ubrr >> 8);
    UBRR0L = UBRRL_VALUE;//(uint8_t)(ubrr & 0xff);
    
    //enable reciever and transmitter
    UCSR0B = ((1 << RXEN0) | (1 << TXEN0));
    
    //set frame format: 8 data, 1 stop bit
    UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00));
}

void transmitByte(uint8_t data){ 
    while ( !(UCSR0A & (1 << UDRE0)) );
    UDR0 = data;
}

uint8_t receiveByte(){
    while ( !(UCSR0A & (1 << RXC0)) );
    return UDR0;
}

void SendBuffer(){  
    int i;
    for (i = 0; i < BS; ++i){
        transmitByte(writeBuffer[i]);
        //receiveByte();
        //_delay_ms(20);
    }
}
