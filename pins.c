#include "main.h"

void initPins(){
    BTN_MODE_DDR        &= ~(1 << BTN_MODE_BIT);
    BTN_START_STOP_DDR  &= ~(1 << BTN_START_STOP_BIT);
    
    SCAN_RED_DDR        |= (1 << SCAN_RED_BIT);
    SCAN_GREEN_DDR      |= (1 << SCAN_GREEN_BIT);
    
    UPLOAD_RED_DDR      |= (1 << UPLOAD_RED_BIT);
    UPLOAD_GREEN_DDR    |= (1 << UPLOAD_GREEN_BIT);
    
    SENSOR_DDR          &= ~(1 << SENSOR_BIT);
    
    CS_DDR              |= (1 << CS_BIT);
}

uint8_t btn_mode_pressed(){
    return 0;
}

uint8_t btn_start_stop_pressed(){
    return 0;
}

void setScanLedColor(enum LedColors color){
    switch (color){
        case RED :
            SCAN_GREEN_PORT &= ~(1 << SCAN_GREEN_BIT);
            SCAN_RED_PORT |= (1 << SCAN_RED_BIT);
            break;
        case YELLOW :
            SCAN_GREEN_PORT |= (1 << SCAN_GREEN_BIT);
            SCAN_RED_PORT |= (1 << SCAN_RED_BIT);
            break;
        case GREEN :
            SCAN_GREEN_PORT |= (1 << SCAN_GREEN_BIT);
            SCAN_RED_PORT &= ~(1 << SCAN_RED_BIT);
            break;
        case NONE :
            SCAN_GREEN_PORT &= ~(1 << SCAN_GREEN_BIT);
            SCAN_RED_PORT &= ~(1 << SCAN_RED_BIT);
            break;
    }
}

void setUploadLedColor(enum LedColors color){
    switch (color){
        case RED :
            UPLOAD_GREEN_PORT &= ~(1 << UPLOAD_GREEN_BIT);
            UPLOAD_RED_PORT |= (1 << UPLOAD_RED_BIT);
            break;
        case YELLOW :
            UPLOAD_GREEN_PORT |= (1 << UPLOAD_GREEN_BIT);
            UPLOAD_RED_PORT |= (1 << UPLOAD_RED_BIT);
            break;
        case GREEN :
            UPLOAD_GREEN_PORT |= (1 << UPLOAD_GREEN_BIT);
            UPLOAD_RED_PORT &= ~(1 << UPLOAD_RED_BIT);
            break;
        case NONE :
            UPLOAD_GREEN_PORT &= ~(1 << UPLOAD_GREEN_BIT);
            UPLOAD_RED_PORT &= ~(1 << UPLOAD_RED_BIT);
            break;
    }
}
