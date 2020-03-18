#include "spi.h"

void chip_select_low(){
#ifdef USE_SPI_LIB
    if (!chip_select_asserted) {
        chip_select_asserted = 1;
        beginTransaction(settings);
    }
#endif    
    SWSPI_PORT &= ~SWSPI_CSEL; //digitalWrite(chipSelectPin_, LOW);
}

void chip_select_high(){
    SWSPI_PORT |= SWSPI_CSEL; //digitalWrite(chipSelectPin_, HIGH);
#ifdef USE_SPI_LIB
    if (chip_select_asserted) {
        chip_select_asserted = 0;
        endTransaction();
    }
#endif
}

void spi_tx_byte(uint8_t b) {
#ifndef USE_SPI_LIB
    SPDR = b;
    while (!(SPSR & (1 << SPIF))) ;
#else
    SDCARD_SPI.transfer(b);
#endif
}

uint8_t spi_rx_byte(void) {
#ifndef USE_SPI_LIB
  spi_tx_byte(0XFF);
  return SPDR;
#else
  return SDCARD_SPI.transfer(0xFF);
#endif
}

void spi_init(void) {
    // set pin modes
    SWSPI_DDR |= SWSPI_CSEL;  //pinMode(chipSelectPin_, OUTPUT);
    SWSPI_PORT |= SWSPI_CSEL; //digitalWrite(chipSelectPin_, HIGH);

    SWSPI_DDR |= (SWSPI_MISO | SWSPI_MOSI | SWSPI_SCLK);//pinMode(SPI_MISO_PIN, INPUT);
                                                      //pinMode(SPI_MOSI_PIN, OUTPUT);
                                                      //pinMode(SPI_SCK_PIN, OUTPUT);
    
    //settings = SPISettings(250000, MSBFIRST, SPI_MODE0);//??
    
    //------------------------ init SPI ------------------
    // Clock settings are defined as follows. Note that this shows SPI2X
    // inverted, so the bits form increasing numbers. Also note that
    // fosc/64 appears twice
    // SPR1 SPR0 ~SPI2X Freq
    //   0    0     0   fosc/2
    //   0    0     1   fosc/4
    //   0    1     0   fosc/8
    //   0    1     1   fosc/16
    //   1    0     0   fosc/32
    //   1    0     1   fosc/64
    //   1    1     0   fosc/64
    //   1    1     1   fosc/128

    // We find the fastest clock that is less than or equal to the
    // given clock rate. The clock divider that results in clock_setting
    // is 2 ^^ (clock_div + 1). If nothing is slow enough, we'll use the
    // slowest (128 == 2 ^^ 7, so clock_div = 6).
    // clock == 250000 == F_CPU / 64
    
    // Pack into the SPISettings class
    // Enable SPI, Master, clock rate f_osc/128
    SPCR |= _BV(SPE) | _BV(MSTR) | (~_BV(DORD)) | _BV(SPR1);
    SPCR &= ~_BV(SPR0) & ~_BV(CPOL) & ~_BV(CPHA);
    // clear double speed
    SPSR &= ~_BV(SPI2X);
}