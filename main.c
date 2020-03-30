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
        case WAIT_FOR_TALK_TO_PC :     
            break;
        case TALK_TO_PC :
            UART_on();
            while (next_state == TALK_TO_PC){
                uint8_t pc_token = rx_byte();
                
                if (next_state != TALK_TO_PC) break;
                
                switch (pc_token){
                    case GET_SIGNAL_LENGTH:
                        sd_read_signal_data();
                        
                        buffer[0] = GET_SIGNAL_LENGTH;
                        buffer[1] = (uint8_t)(signal_length >> 0);
                        buffer[2] = (uint8_t)(signal_length >> 8);
                        buffer[3] = (uint8_t)(signal_length >> 16);
                        buffer[4] = (uint8_t)(signal_length >> 24);
                        
                        send_buffer();
                        break;
                    case GET_DATA:
                        sd_start_read_signal_values();
                        
                        while (1){
                            buffer[0] = GET_DATA;
                            buffer[1] = sd_read_next_byte();
                            buffer[2] = sd_read_next_byte();
                            buffer[3] = 0;
                            buffer[4] = 0;
                        
                            send_buffer();

                            ++sd_cursor.value_num;  

                            if (sd_cursor.value_num >= signal_length) {  
                                buffer[0] = DATA_END;
                                buffer[1] = 0;
                                buffer[2] = 0;
                                buffer[3] = 0;
                                buffer[4] = 0;
                                
                                send_buffer();                            
                                break;
                            }
                        }
                        break;
                    case SAVE_SETTINGS:
                        sd_read_settings();
                        uint8_t channels_mask = rx_byte();
                        sd_buffer[CHANNELS_MASK_BYTE] = channels_mask;
                        
                        if (next_state != TALK_TO_PC) break;
                        sd_write_settings();
                        
                        sd_read_settings();
                        channels_mask = sd_buffer[CHANNELS_MASK_BYTE];
                        
                        buffer[0] = SAVE_SETTINGS;
                        buffer[1] = channels_mask;
                        buffer[2] = 0;
                        buffer[3] = 0;
                        buffer[4] = 0;

                        send_buffer(); 
                        break;
                    case DATA_END:
                        break;
                }
            }
            UART_off();
            break;
        case STOP_TALK_TO_PC :
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