/* 
 * File:   serial.h
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 20:59
 */

#ifndef SERIAL_H
    #define	SERIAL_H

    #include "main.h"

    void upload_signal();
    void UART_on();
    void UART_off();
    
    void tx_byte(uint8_t data);
    uint8_t rx_byte();
    
    void send_buffer();
    
    #define BUFFER_LENGTH 5
    uint8_t buffer[BUFFER_LENGTH];
    
    enum MC_Tokens
    {
      GET_SIGNAL_LENGTH = 1,
      GET_DATA = 2,
      DATA_END = 3,
      SAVE_SETTINGS = 5,
    };

#endif	/* SERIAL_H */

