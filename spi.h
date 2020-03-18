/* 
 * File:   spi.h
 * Author: Рома
 *
 * Created on 18 марта 2020 г., 19:14
 */

#ifndef SPI_H
    #define	SPI_H

    #include "main.h"

    #define SWSPI_PORT					PORTB
    #define SWSPI_PIN					PINB
    #define SWSPI_DDR					DDRB

    #define SWSPI_MISO					_BV(PB4)
    #define SWSPI_MOSI					_BV(PB3)
    #define SWSPI_SCLK					_BV(PB5)
    #define SWSPI_CSEL					_BV(PB2)

    /*
    Define this to make code generate symmetrical
    clock signal, 16 cycles HIGH / 16 cycles LOW
    (4/15 otherwise).
    */
    #define SWSPI_SYMMCLK 1
    #define nop()						{asm volatile ("nop");}

    #define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
    #define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
    #define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

    #define LSBFIRST 0
    #define MSBFIRST 1

    void spi_init(void);
    void chip_select_low();
    void chip_select_high();
    void spi_tx_byte(uint8_t b);
    uint8_t spi_rx_byte(void);

#endif	/* SPI_H */

