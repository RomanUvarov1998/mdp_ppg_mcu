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
        WAIT_FOR_UPLOAD,
        UPLOADIND,
        UPLOADED
    };
    
    enum States state;
    
    void state_set(enum States value);
    void state_pin();
#endif	/* STATE_H */

