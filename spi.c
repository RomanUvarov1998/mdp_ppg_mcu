#include "spi.h"

void SPI_init()
{
    SPI_MASTER | SPI_FOSC_64 | SPI_MODE_0
    // set CS, MOSI and SCK to output
    DDR_SPI |= (1 << CS) | (1 << MOSI) | (1 << SCK);

    // enable pull up resistor in MISO
    DDR_SPI &= ~(1 << MISO);
    PORT_SPI |= (1 << MISO);

    // set SPI params
    SPCR |= (1 << MSTR) | (1 << SPR1 /*f_osc / 66*/);
//    SPCR |= ((uint8_t) (initParams >> 8)) | (1 << SPE);
//    SPSR |= ((uint8_t) initParams);
}

uint8_t SPI_transfer(uint8_t data)
{
    // load data into register
    SPDR = data;

    // Wait for transmission complete
    while(!(SPSR & (1 << SPIF)));

    // return SPDR
    return SPDR;
}