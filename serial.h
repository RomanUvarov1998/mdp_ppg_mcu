/* 
 * File:   serial.h
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 20:59
 */

#ifndef SERIAL_H
    #define	SERIAL_H

    #include "main.h"

    #define BAUD 9600
    #define MYUBRR F_CPU/16/BAUD-1

    /* UART Buffer Defines */
    #define UART_RX_BUFFER_SIZE 8     /* 2,4,8,16,32,64,128 or 256 bytes */
    #define UART_TX_BUFFER_SIZE 16
    #define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)

    #if (UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK)
      #error RX buffer size is not a power of 2
    #endif

    #define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)
    #if (UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK)
      #error TX buffer size is not a power of 2
    #endif

    static uint8_t UART_RxBuf[UART_RX_BUFFER_SIZE];
    static volatile uint8_t UART_RxHead;
    static volatile uint8_t UART_RxTail;
    static uint8_t UART_TxBuf[UART_TX_BUFFER_SIZE];
    static volatile uint8_t UART_TxHead;
    static volatile uint8_t UART_TxTail;

    void stateProcessing();
    void createSignal();
    void initUART(uint16_t ubrr);
    uint8_t receiveByte(void);
    void transmitByte(uint8_t data);
    void SendBuffer();
    void flush();

    #define BS 7
    #define SL 200

    volatile uint32_t signalLength;
    volatile uint16_t signalValues[SL];
    volatile uint32_t sendingValueNum;
    volatile uint8_t writeBuffer[BS];

    #define B_T   49
    #define Q     50
    #define Y     51
    uint8_t response[1];

    enum MessageTypes {
      BEFORE_TX = 1,
      SIGNAL_LENGTH = 2,
      DATA = 3,
      APPROVING = 4,
      DONE = 5,
      SLEEP = 6
    };
    volatile uint8_t stateTx;

#endif	/* SERIAL_H */

