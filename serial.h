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
    void init_UART();

    enum StatesTx {
        BEFORE_TX = 0,
        SIGNAL_LENGTH_01 = 1,
        SIGNAL_LENGTH_23 = 2,
        DATA = 3,
        END = 4,
    };
    
    volatile uint8_t stateTx;

    volatile uint32_t sendingValueNum;

#endif	/* SERIAL_H */

