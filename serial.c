#include "serial.h"

volatile uint32_t signalLength = SL;
volatile uint32_t sendingValueNum = 0;
volatile uint8_t stateTx = BEFORE_TX;

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
                setUploadLedColor(GREEN);
            }
            break;
    }
}

void createSignal(){
    int i;
    for (i = 0; i < signalLength; ++i) signalValues[i] = i;
}

void initUART(uint16_t ubrr){
    unsigned char x;
    
    //set baudrate
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    
    //enable reciever and transmitter
    UCSR0B = ((1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0));
    
    //set frame format: 8 data, 1 stop bit
    UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00));

    /* Flush receive buffer */
    x = 0;          

    UART_RxTail = x;
    UART_RxHead = x;
    UART_TxTail = x;
    UART_TxHead = x;
}

ISR(USART_RX_vect){
    uint8_t data;
    uint8_t tmphead;

    /* Read the received data */
    data = UDR0;                 
    /* Calculate buffer index */
    tmphead = (UART_RxHead + 1) & UART_RX_BUFFER_MASK;
    /* Store new index */
    UART_RxHead = tmphead;      

    if (tmphead == UART_RxTail) {
      /* ERROR! Receive buffer overflow */
    }
    /* Store received data in buffer */
    UART_RxBuf[tmphead] = data;
}

ISR(USART_UDRE_vect){
    uint8_t tmptail;

    /* Check if all data is transmitted */
    if (UART_TxHead != UART_TxTail) {
      /* Calculate buffer index */
      tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK;
      /* Store new index */
      UART_TxTail = tmptail;      
      /* Start transmission */
      UDR0 = UART_TxBuf[tmptail];
    } else {
      /* Disable UDRE interrupt */
      UCSR0B &= ~(1<<UDRIE0);        
    }
}

void transmitByte(uint8_t data){ 
    uint8_t tmphead;
  
    /* Calculate buffer index */
    tmphead = (UART_TxHead + 1) & UART_TX_BUFFER_MASK;
    /* Wait for free space in buffer */
    while (tmphead == UART_TxTail);
    /* Store data in buffer */
    UART_TxBuf[tmphead] = data;
    /* Store new index */
    UART_TxHead = tmphead;
    /* Enable UDRE interrupt */
    UCSR0B |= (1<<UDRIE0);
}

uint8_t receiveByte(){
    uint8_t tmptail;
  
    /* Wait for incoming data */
    while (UART_RxHead == UART_RxTail);
    /* Calculate buffer index */
    tmptail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;
    /* Store new index */
    UART_RxTail = tmptail;

    /* Return data */
    return UART_RxBuf[tmptail];
}

void SendBuffer(){  
    int i;
    for (i = 0; i < BS; ++i){
        transmitByte(writeBuffer[i]);
        receiveByte();
        _delay_ms(10);
    }
}
