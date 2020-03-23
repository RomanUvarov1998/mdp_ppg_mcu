/* 
 * File:   adc.h
 * Author: Рома
 *
 * Created on 19 марта 2020 г., 2:31
 */

#ifndef ADC_H
    #define	ADC_H

    #include "main.h"

    void adc_on();
    void adc_off();
    uint16_t adc_convert(uint8_t channel);

#endif	/* ADC_H */

