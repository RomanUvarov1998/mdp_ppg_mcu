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

#endif	/* SERIAL_H */

