/* 
 * File:   main.c
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 2:00
 */

#include "main.h"

/*
 * 
 */
int main(int argc, char** argv) {
    initPins();
    
    setScanLedColor(NONE);
    
    while(1){
        setScanLedColor(RED);
        setUploadLedColor(RED);
        _delay_ms(500);
        setScanLedColor(YELLOW);
        setUploadLedColor(YELLOW);
        _delay_ms(500);
        setScanLedColor(GREEN);
        setUploadLedColor(GREEN);
        _delay_ms(500);
        setScanLedColor(NONE);
        setUploadLedColor(NONE);
        _delay_ms(500);
    }
    
    return (0);
}

