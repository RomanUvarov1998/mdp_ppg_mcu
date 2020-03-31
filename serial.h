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
      CHANNELS_MASK = 1,
      GET_SIGNAL_LENGTH = 2,
      GET_DATA = 3,
      DATA_END = 4,
      SAVE_SETTINGS = 5,
    };

#endif	/* SERIAL_H */

