#include "state.h"

volatile enum States state = WAIT_FOR_SCAN;
volatile enum States next_state = WAIT_FOR_SCAN;

void state_set(enum States value){
    next_state = value;
}
void state_pin(){
    switch (state){
        case CARD_ERROR :
            set_scan_led_color(RED);
            set_upload_led_lolor(RED); 
            break;
        case WAIT_FOR_SCAN :
            set_scan_led_color(GREEN);
            set_upload_led_lolor(NONE); 
            break;
        case SCANNING :
            set_scan_led_color(RED);
            set_upload_led_lolor(NONE);
            break;
        case WAIT_FOR_UPLOAD :
            set_scan_led_color(NONE);
            set_upload_led_lolor(YELLOW);
            break;
        case UPLOADING :
            set_scan_led_color(NONE);
            set_upload_led_lolor(RED);
            break;
        case UPLOADED :
            set_scan_led_color(NONE);
            set_upload_led_lolor(GREEN);
            break;
    }
}