#include "state.h"

enum States state = WAIT_FOR_SCAN;

void state_set(enum States value){
    state = value;
    state_pin();
}
void state_pin(){
    switch (state){
        case CARD_ERROR :
            setScanLedColor(RED);
            setUploadLedColor(RED); 
            break;
        case WAIT_FOR_SCAN :
            setScanLedColor(GREEN);
            setUploadLedColor(NONE); 
            break;
        case SCANNING :
            setScanLedColor(RED);
            setUploadLedColor(NONE);
            break;
        case WAIT_FOR_UPLOAD :
            setScanLedColor(NONE);
            setUploadLedColor(YELLOW);
            break;
        case UPLOADIND :
            setScanLedColor(NONE);
            setUploadLedColor(RED);
            break;
        case UPLOADED :
            setScanLedColor(NONE);
            setUploadLedColor(GREEN);
            break;
    }
}