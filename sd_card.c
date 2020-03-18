#include "sd_card.h"/** status for card in the ready state */

static uint8_t sd_type;
static uint8_t status_;
static uint8_t inBlock_;
static uint16_t offset_;

#define SIGNAL_DATA_SECTOR 0

#define SDCARD_OK					0x00
#define SDCARD_ERROR				0x01

#define CMD_DATA_TOKEN				0xFE
#define WR_DATA_ACCEPTED			0x04

#define R1_READY_STATE 0X00
/** status for card in the idle state */
#define R1_IDLE_STATE 0X01
/** status bit for illegal command */
#define R1_ILLEGAL_COMMAND 0X04
/** start data token for read or write single block*/
//#define DATA_START_BLOCK 0XFE
///** stop token for write multiple blocks*/
//#define STOP_TRAN_TOKEN 0XFD
///** start data token for write multiple blocks*/
//#define WRITE_MULTIPLE_TOKEN 0XFC
///** mask for data response tokens after a write block operation */
//#define DATA_RES_MASK 0X1F
///** write data accepted token */
//#define DATA_RES_ACCEPTED 0X05


/** init timeout ms */
#define SD_INIT_TIMEOUT 2000
/** erase timeout ms */
//#define SD_ERASE_TIMEOUT 10000
///** read timeout ms */
//#define SD_READ_TIMEOUT 300
///** write time out ms */
//#define SD_WRITE_TIMEOUT 600    


// SD card commands
/** GO_IDLE_STATE - init card in spi mode if CS low */
#define CMD0 0X00
/** SEND_IF_COND - verify SD Memory Card interface operating condition.*/
#define CMD8 0X08
/** SEND_CSD - read the Card Specific Data (CSD register) */
//#define CMD9 0X09
/** SEND_CID - read the card identification information (CID register) */
//#define CMD10 0X0A
/** SEND_STATUS - read the card status register */
//#define CMD13 0X0D
/** SPI Mode */
#define CMD16 0x10						
/** READ_BLOCK - read a single data block from the card */
#define CMD17 0X11
/** WRITE_BLOCK - write a single data block to the card */
#define CMD24 0X18
/** WRITE_MULTIPLE_BLOCK - write blocks of data until a STOP_TRANSMISSION */
//#define CMD25 0X19
/** ERASE_WR_BLK_START - sets the address of the first block to be erased */
//#define CMD32 0X20
/** ERASE_WR_BLK_END - sets the address of the last block of the continuous
    range to be erased*/
//#define CMD33 0X21
/** ERASE - erase all previously selected blocks */
//#define CMD38 0X26
/** APP_CMD - escape for application specific command */
#define CMD55 0X37
/** READ_OCR - read the OCR register of a card */
#define CMD58 0X3A
/** SET_WR_BLK_ERASE_COUNT - Set the number of write blocks to be
     pre-erased before writing */
//#define ACMD23 0X17
/** SD_SEND_OP_COMD - Sends host capacity support information and
    activates the card's initialization process */
#define ACMD41 0X29


// card types
/** Standard capacity V1 SD card */
#define SD_CARD_TYPE_SD1 1
/** Standard capacity V2 SD card */
#define SD_CARD_TYPE_SD2 2
/** High Capacity SD card */
#define SD_CARD_TYPE_SDHC 3

//static volatile uint8_t interruptSave; // temp storage, to restore state
//static volatile uint8_t interruptMode; // 0=none, 1=mask, 2=global
//static volatile uint8_t interruptMask; // which interrupts to mask

//static uint8_t chip_select_asserted = 0;    

#define false 0
#define true 1


static uint8_t cardCommand(uint8_t cmd, uint32_t arg);
static void readEnd(void);
static uint8_t waitNotBusy(uint16_t timeoutMillis);
static uint8_t cardAcmd(uint8_t cmd, uint32_t arg);
static uint8_t setSckRate(uint8_t sckRateID);
static void SendCommandFrame(uint8_t cmd,uint32_t arg);
uint8_t ReceiveR1(void);
void spi_tx(uint8_t data);
uint8_t spi_rx(uint8_t data);


uint8_t initSD(){
    /////////////////////////////////////////////////////////////////// Init SPI
    sd_type = 0;
    // 16-bit init start time allows over a minute
    uint32_t arg;

    spi_init();
    
    //////////////////////////////////////////////////////////////////////////// >= 74 peaks with CS high.
    uint8_t i;
    for (i = 0; i < 10; i++) spi_tx_byte(0XFF);
    //////////////////////////////////////////////////////////////////////////// --- >= 74 peaks with CS high.
    
    chip_select_low(); // Mine
    
    uint16_t t0 = 0;//(uint16_t)millis();
    // command to go idle in SPI mode
    do {
        status_ = cardCommand(CMD0, 0);        
        ++t0;
        if (t0 > SD_INIT_TIMEOUT) {
            //error(SD_CARD_ERROR_CMD0);
            goto fail;
        }
    } while (status_ != R1_IDLE_STATE);
    
    // check SD version
    if ((cardCommand(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND)) {
      sd_type = SD_CARD_TYPE_SD1;
    } else {
      // only need last byte of r7 response
        uint8_t i;
        for (i = 0; i < 4; i++) status_ = spi_rx_byte();
        if (status_ != 0XAA) {
          //error(SD_CARD_ERROR_CMD8);
          goto fail;
        }
        sd_type = SD_CARD_TYPE_SD2;
    }
    // initialize card and send host supports SDHC if SD2
    arg = ( (sd_type == SD_CARD_TYPE_SD2) ? 0X40000000 : 0 );

    while ((status_ = cardAcmd(ACMD41, arg)) != R1_READY_STATE) {
      // check for timeout
        ++t0;
      if (t0 > SD_INIT_TIMEOUT) {
        //error(SD_CARD_ERROR_ACMD41);
        goto fail;
      }
    }
    // if SD2 read OCR register to check for SDHC card
    if (sd_type == SD_CARD_TYPE_SD2) {
      if (cardCommand(CMD58, 0)) {
        //error(SD_CARD_ERROR_CMD58);
        goto fail;
      }
      if ((spi_rx_byte() & 0XC0) == 0XC0) sd_type = SD_CARD_TYPE_SDHC;
      // discard rest of ocr - contains allowed voltage range
      uint8_t i;
      for (i = 0; i < 3; i++) spi_rx_byte();
    }
    
    chip_select_high(); // Mine
    
#ifndef SOFTWARE_SPI
    return setSckRate(1); //sckRateID == 1
#else  // SOFTWARE_SPI
    return 0;//OK
#endif  // SOFTWARE_SPI

fail:
    chip_select_high();
    return 1;//Not Ok
    
    ///////////////////////////////////////////////////////////////////////////// --init SPI
}  


static uint8_t cardCommand(uint8_t cmd, uint32_t arg) {
  // end read if in partialBlockRead mode
  readEnd();

  // select card
  chip_select_low();

  // wait up to 300 ms if busy
  waitNotBusy(300);

  // send command
  spi_tx_byte(cmd | 0x40);

  // send argument
  int8_t s;
  for (s = 24; s >= 0; s -= 8) spi_tx_byte(arg >> s);

  // send CRC
  uint8_t crc = 0XFF;
  if (cmd == CMD0) crc = 0X95;  // correct crc for CMD0 with arg 0
  if (cmd == CMD8) crc = 0X87;  // correct crc for CMD8 with arg 0X1AA
  spi_tx_byte(crc);

  // wait for response
  uint8_t i;
  for (i = 0; ((status_ = spi_rx_byte()) & 0X80) && i != 0XFF; i++) ;
  return status_;
}

static void readEnd(void) {
  if (inBlock_) {
      // skip data and crc
    // optimize skip for hardware
    SPDR = 0XFF;
    while (offset_++ < 513) {
      while (!(SPSR & (1 << SPIF)));
      SPDR = 0XFF;
    }
    // wait for last crc byte
    while (!(SPSR & (1 << SPIF)));
    chip_select_high();
    inBlock_ = 0;
  }
}

static uint8_t waitNotBusy(uint16_t timeoutMillis) {
  uint16_t t0 = 0;
  do {
    if (spi_rx_byte() == 0XFF) return true;
  }
  while (t0 < timeoutMillis);
  return false;
}

static uint8_t cardAcmd(uint8_t cmd, uint32_t arg) {
    cardCommand(CMD55, 0);
    return cardCommand(cmd, arg);
  }

static uint8_t setSckRate(uint8_t sckRateID) {
  if (sckRateID > 6) {
    //error(SD_CARD_ERROR_SCK_RATE);
    return false;
  }
#ifndef USE_SPI_LIB
  // see avr processor datasheet for SPI register bit definitions
  if ((sckRateID & 1) || sckRateID == 6) {
    SPSR &= ~(1 << SPI2X);
  } else {
    SPSR |= (1 << SPI2X);
  }
  SPCR &= ~((1 <<SPR1) | (1 << SPR0));
  SPCR |= (sckRateID & 4 ? (1 << SPR1) : 0)
    | (sckRateID & 2 ? (1 << SPR0) : 0);
#else // USE_SPI_LIB
  switch (sckRateID) {
    case 0:  settings = SPISettings(25000000, MSBFIRST, SPI_MODE0); break;
    case 1:  settings = SPISettings(4000000, MSBFIRST, SPI_MODE0); break;
    case 2:  settings = SPISettings(2000000, MSBFIRST, SPI_MODE0); break;
    case 3:  settings = SPISettings(1000000, MSBFIRST, SPI_MODE0); break;
    case 4:  settings = SPISettings(500000, MSBFIRST, SPI_MODE0); break;
    case 5:  settings = SPISettings(250000, MSBFIRST, SPI_MODE0); break;
    default: settings = SPISettings(125000, MSBFIRST, SPI_MODE0);
  }
#endif // USE_SPI_LIB
  return true;
}



void createAndSaveSignal(uint32_t length){    
    uint16_t i;
    
    // clear buffer, all bytes = 0
    for (i = 0; i < SDCARD_RW_BLOCK_SIZE; ++i) sd_buffer[i] = 0;
    
    //0:3 signal length
    sd_buffer[0] = (uint8_t)(length >> 0);
    sd_buffer[1] = (uint8_t)(length >> 8);
    sd_buffer[2] = (uint8_t)(length >> 16);
    sd_buffer[3] = (uint8_t)(length >> 24);
    
    //4 Fs
    sd_buffer[4] = (uint8_t)(250);
    
    SDCARD_WriteBlock(sd_buffer, SIGNAL_DATA_SECTOR);
                
    sd_cursor.sector_num = 1;
    sd_cursor.value_num = 0;
    sd_cursor.byte_num = 0;
    
    while (sd_cursor.value_num < length){
        sd_buffer[sd_cursor.byte_num] =       (uint8_t)(sd_cursor.value_num >> 0);
        sd_buffer[sd_cursor.byte_num + 1] =   (uint8_t)(sd_cursor.value_num >> 8);
        
        sd_cursor.byte_num += 2;
        if (sd_cursor.byte_num >= SDCARD_RW_BLOCK_SIZE || sd_cursor.value_num >= length - 1){
            SDCARD_WriteBlock(sd_buffer, sd_cursor.sector_num);
            
            set_scan_led_color(GREEN);
            sd_cursor.byte_num = 0;
            ++sd_cursor.sector_num;
        }
        
        ++sd_cursor.value_num;
    }
}

uint8_t SDCARD_ReadBlock(uint8_t block[SDCARD_RW_BLOCK_SIZE],uint32_t start_addr) {
  uint8_t resp;
  uint16_t byte_count;
  
  //Issue block read
  SendCommandFrame(CMD17, start_addr);
  resp=ReceiveR1();
        
  if (resp != R1_READY_STATE)
  {
    return SDCARD_ERROR;
  } 
              
  while (resp != CMD_DATA_TOKEN)
  {
    resp = spi_rx(0xFF);
  }  

  for (byte_count = 0; byte_count < SDCARD_RW_BLOCK_SIZE; byte_count++)
  {
    block[byte_count] = spi_rx(0xFF);
  }

  //Skipping CRC
  spi_tx(0xFF);
  spi_tx(0xFF);
  
  return SDCARD_OK;
}

uint8_t SDCARD_WriteBlock(uint8_t block[SDCARD_RW_BLOCK_SIZE],uint32_t start_addr) {
    uint8_t resp;
    uint16_t i;

    #if SDCARD_RW_BLOCK_SIZE > 512
    #error Block size currently must not exceed 512 bytes.
    #endif

    //Set the R/W block size
    //Required because write mode only supports 512 byte blocks
    SendCommandFrame(CMD16, 512);
    resp = ReceiveR1();
    if (resp != 0)
    {
        return SDCARD_ERROR;
    }

    //Issue single block write
    SendCommandFrame(CMD24,start_addr);
    resp = ReceiveR1();

    if (resp != R1_READY_STATE)
    {
        return SDCARD_ERROR;
    }

    spi_tx(0xFF);
    spi_tx(CMD_DATA_TOKEN);

    //Perform actual data transfer
    for (i=0; i<SDCARD_RW_BLOCK_SIZE; i++)
    {
      spi_tx(block[i]);
    }

    #if SDCARD_RW_BLOCK_SIZE<512

    #warning Added alignment code to block write routine.
    //Send alignment data
    for (i=0; i<512-SDCARD_RW_BLOCK_SIZE; i++)
    {
      SWSPI_ByteRxTx(0xFF);
    }
    #endif

    //Send dummy CRC
    spi_tx(0xFF);
    spi_tx(0xFF);

    //Check write status
    resp = spi_rx(0xFF);

    if (!(resp & WR_DATA_ACCEPTED))
    {
      return SDCARD_ERROR;
    }

    resp=0;

    //Waiting while the card is busy
    while (resp==0)
    {
      resp = spi_rx(0xFF);
    }

    //Set read block size back
    SendCommandFrame(CMD16,SDCARD_RW_BLOCK_SIZE);
    resp = ReceiveR1();

    if (resp != R1_READY_STATE)
    {
      return SDCARD_ERROR;
    }

    return SDCARD_OK;
}

void SendCommandFrame(uint8_t cmd,uint32_t arg) {
//  //First byte - command itself
//  SWSPI_ByteRxTx(0x40 | cmd);
//
//  //Then argument, four bytes MSB first
//  SWSPI_ByteRxTx((arg & 0xFF000000UL) >> 24);
//  SWSPI_ByteRxTx((arg & 0x00FF0000UL) >> 16);
//  SWSPI_ByteRxTx((arg & 0x0000FF00UL) >> 8);
//  SWSPI_ByteRxTx((arg & 0x000000FFUL) >> 0);
//
//  //Then CRC hardcoded for CMD0 (ignored in SPI mode)
//  SWSPI_ByteRxTx(0x95);
    spi_tx(0x40 | cmd);
    spi_tx((uint8_t)(arg >> 24));
    spi_tx((uint8_t)(arg >> 16));
    spi_tx((uint8_t)(arg >> 8));
    spi_tx((uint8_t)(arg >> 0));
    spi_tx(0x95);
}

uint8_t ReceiveR1(void) {
    uint8_t R1 = 0xFF;
    uint32_t i = 0;

    //The SD card will answer in 0 to 8 byte periods
    //The first bit of a valid answer will be zero.
    while (R1 == 0xFF)
    {
        R1 = spi_rx(0xFF);

        ++i;

        if (i >= 0xFF)
        {
            //Something's wrong - timeout error
            break;
        }
    }

    return R1;
}

void spi_tx(uint8_t data){
    SWSPI_PORT &= ~SWSPI_CSEL;
    
    SPDR = data;
    while ( !(SPSR & (1 << SPIF)) );
    
    SWSPI_PORT |= SWSPI_CSEL;
}

uint8_t spi_rx(uint8_t data){
    uint8_t rx;
    
    SWSPI_PORT &= ~SWSPI_CSEL;
    
    SPDR = data;
    while ( !(SPSR & (1 << SPIF)) );
    
    rx = SPDR;
    
    SWSPI_PORT |= SWSPI_CSEL;
    
    return rx;
}

void reset_sd_cursor(){ 
    SDCARD_ReadBlock(sd_buffer, SIGNAL_DATA_SECTOR);
    
    signal_length = 0;
    signal_length |= ((uint32_t)sd_buffer[0]) << 0;
    signal_length |= ((uint32_t)sd_buffer[1]) << 8;
    signal_length |= ((uint32_t)sd_buffer[2]) << 16;
    signal_length |= ((uint32_t)sd_buffer[3]) << 24;
            
    sd_cursor.sector_num = 1;
    sd_cursor.value_num = 0;
    sd_cursor.byte_num = 0;
    
    SDCARD_ReadBlock(sd_buffer, sd_cursor.sector_num);
}

uint8_t sd_next_byte(){
    uint8_t result = sd_buffer[sd_cursor.byte_num];
    
    ++sd_cursor.byte_num;
    
    if (sd_cursor.byte_num >= SDCARD_RW_BLOCK_SIZE){
//        if ( !sd_has_next_byte() ){
//            setScanLedColor(RED);
//            setUploadLedColor(RED);
//            return 0;
//        }
        
        sd_cursor.byte_num = 0;
        ++sd_cursor.sector_num;
        SDCARD_ReadBlock(sd_buffer, sd_cursor.sector_num);
    }
    
    return result;
}

uint8_t sd_has_next_byte(){
    return (sd_cursor.value_num < signal_length);
}