#include "btns.h"

void init_btns(){
    EICRA |= ((1 << ISC01) | (1 << ISC00) | 
            (1 << ISC11) | (1 << ISC10)); //on rising edge
    
    EIMSK |= ((1 << INT0) | (1 << INT1));//PORTD2 and PORTD3 pins

    PORTD |= ((1 << PORTD2) | (1 << PORTD3));
}

// btn "MODE"
ISR(INT1_vect){
    //cli();
    switch (state){
          case CARD_ERROR :
              break;
          case WAIT_FOR_SCAN :       
              state_set(WAIT_FOR_UPLOAD);
              break;
          case SCANNING :
              break;
          case WAIT_FOR_UPLOAD :
              state_set(WAIT_FOR_SCAN); 
              break;
          case TALK_TO_PC :
              break;
          case STOP_TALK_TO_PC :
              state_set(WAIT_FOR_SCAN); 
              break;
      }
    //sei();
}

// btn "START/STOP"
ISR(INT0_vect){
    switch (state){
        case CARD_ERROR :
            break;
        case WAIT_FOR_SCAN :     
            state_set(SCANNING); 
            break;
        case SCANNING :
            state_set(WAIT_FOR_SCAN); 
            break;
        case WAIT_FOR_UPLOAD :
            state_set(TALK_TO_PC); 
            break;
        case TALK_TO_PC :
            state_set(STOP_TALK_TO_PC); 
            break;
        case STOP_TALK_TO_PC :
            state_set(TALK_TO_PC); 
            break;
    }
}
