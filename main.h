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
    #include "pins.h"
    #include "state.h"
    #include "avr/interrupt.h"
    #include "serial.h"

    void state_transit();
#endif

