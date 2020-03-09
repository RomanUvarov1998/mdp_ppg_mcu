/* 
 * File:   main.c
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 2:00
 */

#include "main.h"

int main(int argc, char** argv) {
    initPins();   
    
    while(1){
        state_pin();
        state_transit();
    }

    return (0);
}

void state_transit(){    
    switch (state){
        case CARD_ERROR :
            break;
        case WAIT_FOR_SCAN :       
            if (btn_mode_pressed()){ 
                state_set(WAIT_FOR_UPLOAD);
                _delay_ms(1000); 
                break;
            }
            if (btn_start_stop_pressed()){ 
                state_set(SCANNING); 
                _delay_ms(1000);
                break; }
            break;
        case SCANNING :
            //can't change mode
            if (btn_start_stop_pressed()){ 
                state_set(WAIT_FOR_SCAN); 
                _delay_ms(1000); 
                break; 
            }
            break;
        case WAIT_FOR_UPLOAD :
            if (btn_mode_pressed()){ 
                state_set(WAIT_FOR_SCAN); 
                _delay_ms(1000); 
                break; 
            }
            if (btn_start_stop_pressed()){ 
                state_set(UPLOADIND); 
                initUART(MYUBRR);
                sei();    
                createSignal();
                sendingValueNum = 0;
                stateTx = BEFORE_TX;
                _delay_ms(1000); 
                break;
            }
            break;
        case UPLOADIND :
            if (btn_mode_pressed()){ 
                state_set(WAIT_FOR_UPLOAD); 
                _delay_ms(1000); break; 
            }
            if (btn_start_stop_pressed()){ 
                state_set(WAIT_FOR_UPLOAD); 
                _delay_ms(1000);
                break; 
            }
            stateProcessing();
            if (stateTx == END) {
                state_set(UPLOADED);
                _delay_ms(1000); 
                break;
            }
            break;
        case UPLOADED :
            if (btn_mode_pressed()){ 
                state_set(WAIT_FOR_SCAN); 
                _delay_ms(1000); 
                break; 
            }
            //nothing to start and stop
            break;
    }
}

