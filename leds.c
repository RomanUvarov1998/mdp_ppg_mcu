#include "leds.h"

void init_leds(){    
    SCAN_RED_DDR        |= (1 << SCAN_RED_BIT);
    SCAN_GREEN_DDR      |= (1 << SCAN_GREEN_BIT);
    
    UPLOAD_RED_DDR      |= (1 << UPLOAD_RED_BIT);
    UPLOAD_GREEN_DDR    |= (1 << UPLOAD_GREEN_BIT);
}

void set_scan_led_color(enum LedColors color){
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

void set_upload_led_lolor(enum LedColors color){
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
