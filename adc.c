#include "adc.h"

void adc_on(){
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (0 << ADATE) | (0 << ADIF) | (0 << ADIE) 
            | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //prescaler 125 kHz
    /*ADCSRB = (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0);*/
    /*ADMUX = (1 << REFS1) | (1 << REFS0) // V ref
            | (0 << ADLAR) 
            | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);*/ //choose
    ADMUX = (1 << REFS0); //AVCC with external capasitor at AREF pin
    DDRC &= ~(1 << PC0);
}

void adc_off(){
    ADCSRA &= ~(1 << ADEN);
}

uint16_t adc_convert(uint8_t channel){  
    ADMUX &= 0xF0;
    ADMUX |= (channel & 0x0F);
    
    _delay_ms(1);
    
    uint16_t value;
    
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));  
//    while ( !(ADCSRA & (1 << ADIF)) );   
    
    value = ADCL;
    value |= ( ((uint16_t)ADCH) << 8 );
    
    ADCSRA |= (1 << ADIF);
    
//    return ((uint16_t)ADC);
    return value;
}