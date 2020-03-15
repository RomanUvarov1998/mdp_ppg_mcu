/* 
 * File:   pins.h
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 2:06
 */

#ifndef PINS_H

    #define	PINS_H

    #include "main.h"

    //pin 9
    #define SCAN_RED_PORT   PORTB
    #define SCAN_RED_BIT    1
    #define SCAN_RED_DDR    DDRB

    //pin 8
    #define SCAN_GREEN_PORT   PORTB
    #define SCAN_GREEN_BIT    0
    #define SCAN_GREEN_DDR    DDRB

    //pin 6
    #define UPLOAD_RED_PORT   PORTD
    #define UPLOAD_RED_BIT    6
    #define UPLOAD_RED_DDR    DDRD

    //pin 5
    #define UPLOAD_GREEN_PORT   PORTD
    #define UPLOAD_GREEN_BIT    5
    #define UPLOAD_GREEN_DDR    DDRD

    enum LedColors { RED, YELLOW, GREEN, NONE };

    void initPins();
    void setScanLedColor(enum LedColors color);
    void setUploadLedColor(enum LedColors color);

#endif	/* PINS_H */

