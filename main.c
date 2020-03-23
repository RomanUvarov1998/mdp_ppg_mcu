/* 
 * File:   main.c
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 2:00
 */

#include "main.h"
#define nop  asm("nop");

static void state_transit();

void disable_not_needed_interrupts(){
    PCMSK2 = 0;
    PCMSK1 = 0;
    PCMSK0 = 0;

    WDTCSR &= ~_BV(WDIE);

    TIMSK0 &= (~_BV(OCIE0B) & ~_BV(OCIE0A) & ~_BV(TOIE0));
    TIMSK1 &= (~_BV(ICIE1) & ~_BV(OCIE1B) & ~_BV(OCIE1A) & ~_BV(TOIE1));
    TIMSK2 &= (~_BV(OCIE2B) & ~_BV(OCIE2A) & ~_BV(TOIE2));

    SPCR &= ~_BV(SPIE);

    UCSR0B &= (~_BV(RXCIE0) & ~_BV(TXCIE0) & ~_BV(UDRIE0));

    ADCSRA &= ~_BV(ADIE);

    EECR &= (~_BV(EERIE) & ~_BV(EEMPE) & ~_BV(EEPE) & ~_BV(EERE));

    ACSR  &= (~_BV(ACIE) & ~_BV(ACIC));

    TWCR &= ~_BV(TWIE);

    MCUSR = 0;
}

ISR(ADC_vect){ nop }
ISR(ANALOG_COMP_vect){ nop }
ISR(EE_READY_vect){ nop }
ISR(PCINT0_vect){ nop }
ISR(PCINT1_vect){ nop }
ISR(PCINT2_vect){ nop }
ISR(SPI_STC_vect){ nop }
ISR(SPM_READY_vect){ nop }
ISR(TIMER0_COMPB_vect){ nop }
ISR(TIMER0_OVF_vect){ nop }
ISR(TIMER1_CAPT_vect){ nop }
ISR(TIMER1_COMPA_vect){ nop }
ISR(TIMER1_COMPB_vect){ nop }
ISR(TIMER1_OVF_vect){ nop }
ISR(TIMER2_OVF_vect){ nop }
ISR(TWI_vect){ nop }
ISR(USART_RX_vect){ nop }
ISR(USART_TX_vect){ nop }
ISR(USART_UDRE_vect){ nop }
ISR(WDT_vect){ nop }
    
int main(int argc, char** argv) {
    disable_not_needed_interrupts();
    init_leds();     
    init_btns();
    
#if USE_SD_CARD
    SPI_init(SPI_MASTER | SPI_FOSC_64 | SPI_MODE_0);
    
    SD_init();  
#endif
    
    set_scan_led_color(GREEN);
    set_upload_led_lolor(GREEN);
    _delay_ms(500);
    set_scan_led_color(RED);
    set_upload_led_lolor(RED);
    _delay_ms(500);
    
//    while(1);
    
//    set_upload_led_lolor(RED);    
//    createAndSaveSignal(4500);
//    set_upload_led_lolor(GREEN);   
//    _delay_ms(1000);
    
    sei();
    
    while(1) state_transit();

    return (0);
}

static void state_transit(){ 
    //MCUSR = 0;  
    state = next_state;
    
    state_leds();
    
    switch (state){
        case CARD_ERROR :
            break;
        case WAIT_FOR_SCAN :       
            break;
        case SCANNING :  
            adc_on();
            scan_while_btn_pressed();
            adc_off();
            break;
        case WAIT_FOR_UPLOAD :     
            break;
        case UPLOADING :
            UART_on();
            upload_signal();
            UART_off();
            break;
        case UPLOADED :
            break;
    }
}

void Error(){
#if NOTIFY
    while (1){
        set_scan_led_color(RED);
        set_upload_led_lolor(RED);
    }
#endif
}

void Success(){
#if NOTIFY
    set_scan_led_color(NONE);
    set_upload_led_lolor(NONE);        
    _delay_ms(300);
    set_scan_led_color(GREEN);
    set_upload_led_lolor(GREEN);     
    _delay_ms(100);
    
    set_scan_led_color(NONE);
    set_upload_led_lolor(NONE);  
    _delay_ms(300); 
#endif
}

void Mark(){
#if NOTIFY
    while (1){
        set_scan_led_color(RED);
        set_upload_led_lolor(YELLOW);
    }
#endif
}