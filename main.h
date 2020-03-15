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
    //#define MYUBRR 104//F_CPU/16/BAUD-1
    #define 	BAUD_TOL   1 
//    #define 	UBRR_VALUE 
//    #define 	UBRRL_VALUE 
//    #define 	UBRRH_VALUE 
    #define 	USE_2X   0
    #include "util/setbaud.h"
    #include "serial.h"

    #include "btns.h"
    
    //SD library
    #include "string.h"
    #include "SD/diskio.h"
    #include "SD/pff.h"
    #include "SD/pffconf.h"

    void state_transit();
#endif

