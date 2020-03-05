/* 
 * File:   pins.h
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 2:06
 */

#ifndef PINS_H

    #define	PINS_H

    #include "main.h"

    //pin 4
    #define BTN_MODE_PORT   PORTD
    #define BTN_MODE_BIT    4
    #define BTN_MODE_DDR    DDRD

    //pin 2
    #define BTN_START_STOP_PORT   PORTD
    #define BTN_START_STOP_BIT    2
    #define BTN_START_STOP_DDR    DDRD

    //pin 10
    #define SCAN_RED_PORT   PORTB
    #define SCAN_RED_BIT    2
    #define SCAN_RED_DDR    DDRB

    //pin 9
    #define SCAN_GREEN_PORT   PORTB
    #define SCAN_GREEN_BIT    1
    #define SCAN_GREEN_DDR    DDRB

    //pin 6
    #define UPLOAD_RED_PORT   PORTD
    #define UPLOAD_RED_BIT    6
    #define UPLOAD_RED_DDR    DDRD

    //pin 5
    #define UPLOAD_GREEN_PORT   PORTD
    #define UPLOAD_GREEN_BIT    5
    #define UPLOAD_GREEN_DDR    DDRD

    //pin A0
    #define SENSOR_PORT   PORTC
    #define SENSOR_BIT    0
    #define SENSOR_DDR    DDRC

    //pin 8
    #define CS_PORT   PORTB
    #define CS_BIT    0
    #define CS_DDR    DDRB

    void initPins();
    uint8_t btn_mode_pressed();
    uint8_t btn_start_stop_pressed();
    void setScanLedColor(enum LedColors color);
    void setUploadLedColor(enum LedColors color);

#endif	/* PINS_H */

