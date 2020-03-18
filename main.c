/* 
 * File:   main.c
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 2:00
 */

#include "main.h"

static void state_transit();
    
int main(int argc, char** argv) {
    init_leds();     
    init_btns();
    
    initSD(); 
    
//    set_upload_led_lolor(RED);    
//    createAndSaveSignal(4500);
//    set_upload_led_lolor(GREEN);   
//    _delay_ms(1000);
    
    sei();
    
    while(1) state_transit();

    return (0);
}

static void state_transit(){   
    state = next_state;
    
    state_pin();
    
    switch (state){
        case CARD_ERROR :
            break;
        case WAIT_FOR_SCAN :       
            break;
        case SCANNING :
            break;
        case WAIT_FOR_UPLOAD :     
            break;
        case UPLOADING :             
            upload_signal();
            break;
        case UPLOADED :
            break;
    }
}






