/* 
 * File:   state.h
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 4:08
 */

#ifndef STATE_H
    #define	STATE_H

    #include "main.h"
    
    enum States {
        CARD_ERROR,
        WAIT_FOR_SCAN,
        SCANNING,
        WAIT_FOR_TALK_TO_PC,
        TALK_TO_PC,
        STOP_TALK_TO_PC
    };
    
    volatile enum States state;
    volatile enum States next_state;
    
    void state_set(enum States value);
    void state_leds();
#endif	/* STATE_H */

