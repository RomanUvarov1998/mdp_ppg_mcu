/* 
 * File:   main.h
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 2:01
 */

#ifndef MAIN_H
    #define	MAIN_H

    #define F_CPU 16000000UL
    #include "avr/io.h"
    #include "util/delay.h"
    #include "stdint.h"    
    #include "leds.h"
    #include "state.h"
    #include "avr/interrupt.h"

    #define BAUD 9600
    #define BAUD_TOL   1 
    #define USE_2X   0
    #include "util/setbaud.h"
    #include "serial.h"

    #include "btns.h"

    #include "string.h"
    #include "spi.h"
    #include "sd_card.h"
    
#endif

