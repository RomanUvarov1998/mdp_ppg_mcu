#include "sd_card.h"/** status for card in the ready state */

/*******************************************************************************
 Initialize SD card
*******************************************************************************/
uint8_t SD_init()
{
    uint8_t res[5], cmdAttempts = 0;
    
    SD_powerUpSeq();
    Success();

    while((res[0] = SD_goIdleState()) != SD_IN_IDLE_STATE)
    {
        cmdAttempts++;
        if(cmdAttempts == CMD0_MAX_ATTEMPTS)
        {
            Error();
            return SD_ERROR;
        }
    }
#if TRACE_SD_INIT
    Success();
#endif

    _delay_ms(1);

    SD_sendIfCond(res);
    if(res[0] != SD_IN_IDLE_STATE)
    {
        Error();
        return SD_ERROR;
    }
#if TRACE_SD_INIT
    Success();
#endif

    if(res[4] != 0xAA)
    {
        Error();
        return SD_ERROR;
    }
#if TRACE_SD_INIT
    Success();
#endif

    uint16_t a = 0;
    do
    {
        if(a >= 0xFFF)
        {
            Error();
            return SD_ERROR;
        }

        res[0] = SD_sendApp();
        if(SD_R1_NO_ERROR(res[0]))
        {
            res[0] = SD_sendOpCond();
        }

        _delay_ms(3);

        a++;
    }
    while(res[0] != SD_READY);

    _delay_ms(1);

    SD_readOCR(res);
    
    //if (res[3] & 0x40) Mark();

#if TRACE_SD_INIT
    Success();
#endif
    return SD_SUCCESS;
}

/*******************************************************************************
 Run power up sequence
*******************************************************************************/
void SD_powerUpSeq()
{
    //turn on pin 
    DDRD |= (1 << PD7);
    PORTD |= (1 << PD7);
    
    // make sure card is deselected
    CS_DISABLE();

    // give SD card time to power up
    _delay_ms(10);
    
    // select SD card
    SPI_transfer(0xFF);
    CS_DISABLE();

    // send 80 clock cycles to synchronize
    uint8_t i;
    for(i = 0; i < SD_INIT_CYCLES; i++)
        SPI_transfer(0xFF);
}

/*******************************************************************************
 Send command to SD card
*******************************************************************************/
void SD_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    // transmit command to sd card
    SPI_transfer(cmd|0x40);

    // transmit argument
    SPI_transfer((uint8_t)(arg >> 24));
    SPI_transfer((uint8_t)(arg >> 16));
    SPI_transfer((uint8_t)(arg >> 8));
    SPI_transfer((uint8_t)(arg));

    // transmit crc
    SPI_transfer(crc|0x01);
}

/*******************************************************************************
 Read R1 from SD card
*******************************************************************************/
uint8_t SD_readRes1()
{
    uint8_t i = 0, res1;

    // keep polling until actual data received
    while((res1 = SPI_transfer(0xFF)) == 0xFF)
    {
        i++;

        // if no data received for 8 bytes, break
        if(i > 8) break;
    }

    return res1;
}

/*******************************************************************************
 Read R2 from SD card
*******************************************************************************/
void SD_readRes2(uint8_t *res)
{
    // read response 1 in R2
    res[0] = SD_readRes1();

    // read final byte of response
    res[1] = SPI_transfer(0xFF);
}

/*******************************************************************************
 Read R3 from SD card
*******************************************************************************/
void SD_readRes3(uint8_t *res)
{
    // read response 1 in R3
    res[0] = SD_readRes1();

    // if error reading R1, return
    if(res[0] > 1) return;

    // read remaining bytes
    SD_readBytes(res + 1, R3_BYTES);
}

/*******************************************************************************
 Read R7 from SD card
*******************************************************************************/
void SD_readRes7(uint8_t *res)
{
    // read response 1 in R7
    res[0] = SD_readRes1();

    // if error reading R1, return
    if(res[0] > 1) return;

    // read remaining bytes
    SD_readBytes(res + 1, R7_BYTES);
}

/*******************************************************************************
 Read specified number of bytes from SD card
*******************************************************************************/
void SD_readBytes(uint8_t *res, uint8_t n)
{
    while(n--) *res++ = SPI_transfer(0xFF);
}

/*******************************************************************************
 Command Idle State (CMD0)
*******************************************************************************/
uint8_t SD_goIdleState()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(CMD0, CMD0_ARG, CMD0_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

/*******************************************************************************
 Send Interface Conditions (CMD8)
*******************************************************************************/
void SD_sendIfCond(uint8_t *res)
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD8
    SD_command(CMD8, CMD8_ARG, CMD8_CRC);

    // read response
    SD_readRes7(res);
    //SD_readBytes(res + 1, R7_BYTES);

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);
}

/*******************************************************************************
 Read Status
*******************************************************************************/
void SD_sendStatus(uint8_t *res)
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD13
    SD_command(CMD13, CMD13_ARG, CMD13_CRC);

    // read response
    SD_readRes2(res);

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);
}

/*******************************************************************************
 Reads OCR from SD Card
*******************************************************************************/
void SD_readOCR(uint8_t *res)
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    uint8_t tmp = SPI_transfer(0xFF);

    if(tmp != 0xFF) while(SPI_transfer(0xFF) != 0xFF) ;

    // send CMD58
    SD_command(CMD58, CMD58_ARG, CMD58_CRC);

    // read response
    SD_readRes3(res);

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);
}

/*******************************************************************************
 Send application command (CMD55)
*******************************************************************************/
uint8_t SD_sendApp()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(CMD55, CMD55_ARG, CMD55_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

/*******************************************************************************
 Send operating condition (ACMD41)
*******************************************************************************/
uint8_t SD_sendOpCond()
{
    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD0
    SD_command(ACMD41, ACMD41_ARG, ACMD41_CRC);

    // read response
    uint8_t res1 = SD_readRes1();

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}


uint8_t token;

#if TRACE_BLOCK_READ_WRITE
static void IndicateSDWrite(){
    switch (token){
        case 0x05: 
            Success();
            break;
        case 0x00://busy timeout
            Mark();
            break;
        case 0xFF://response timeout
            Error();
            break;
        case 0x0B://data rejected due to a CRC error
            while (1){
                set_scan_led_color(RED);
                set_upload_led_lolor(GREEN);
                _delay_ms(500);
                set_scan_led_color(GREEN);
                set_upload_led_lolor(RED);
                _delay_ms(500);
            }
            break;
        case 0x0D://data rejected due to a write error
            while (1){
                set_scan_led_color(RED);
                set_upload_led_lolor(NONE);
                _delay_ms(500);
                set_scan_led_color(NONE);
                set_upload_led_lolor(RED);
                _delay_ms(500);
            }
            break;
    }
}
#endif

/*******************************************************************************
 Read single 512 byte block
 token = 0xFE - Successful read
 token = 0x0X - Data error
 token = 0xFF - timeout
*******************************************************************************/
uint8_t SD_readSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token)
{
    uint8_t res1, read;
    uint16_t readAttempts;

    // set token to none
    *token = 0xFF;

    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);

    // send CMD17
    SD_command(CMD17, addr, CMD17_CRC);

    // read R1
    res1 = SD_readRes1();

    // if response received from card
    if(res1 != 0xFF)
    {
        // wait for a response token (timeout = 100ms)
        readAttempts = 0;
        while(++readAttempts != SD_MAX_READ_ATTEMPTS)
            if((read = SPI_transfer(0xFF)) != 0xFF) break;

        // if response token is 0xFE
        if(read == SD_17_18_24_TOKEN)
        {
            // read 512 byte block
            uint16_t i;
            for(i = 0; i < SD_BLOCK_LEN; i++) *buf++ = SPI_transfer(0xFF);

            // read 16-bit CRC
            SPI_transfer(0xFF);
            SPI_transfer(0xFF);
        }

        // set token to card response
        *token = read;
    }

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}

#define SD_MAX_WRITE_ATTEMPTS   3907

/*******************************************************************************
Write single 512 byte block
token = 0x00 - busy timeout
token = 0x05 - data accepted
token = 0xFF - response timeout
*******************************************************************************/
uint8_t SD_writeSingleBlock(uint32_t addr, const uint8_t *buf, uint8_t *token) {
    uint16_t readAttempts;
    uint8_t res1, read;

    // set token to none
    *token = 0xFF;

    // assert chip select
    SPI_transfer(0xFF);
    CS_ENABLE();
    SPI_transfer(0xFF);
    
//    do {
        // send CMD24
        SD_command(CMD24, addr, CMD24_CRC);
        _delay_ms(1);    

        // read response
        res1 = SD_readRes1();        
        _delay_ms(1);    
//    } while (res1 != SD_READY);
    
#if TRACE_BLOCK_READ_WRITE
    set_scan_led_color(NONE);
    set_upload_led_lolor(GREEN);
    _delay_ms(200);
    set_scan_led_color(GREEN);
    set_upload_led_lolor(GREEN);
    _delay_ms(200);
    set_scan_led_color(GREEN);
    set_upload_led_lolor(NONE);
    _delay_ms(200);
    set_scan_led_color(NONE);
    set_upload_led_lolor(NONE);
#endif

    // if no error
    if(res1 == SD_READY)
    {
        // send start token
        SPI_transfer(SD_17_18_24_TOKEN);

        // write buffer to card
        uint16_t i;
        for(i = 0; i < SD_BLOCK_LEN; i++) {
            SPI_transfer(buf[i]);  
        }
        
        ///CRC
        SPI_transfer(0x00);
        SPI_transfer(0x00);
        
        // wait for a response (timeout = 250ms)
        readAttempts = 0;
        while(++readAttempts != SD_MAX_WRITE_ATTEMPTS || 1 == 1){ 
            _delay_ms(1);
            if((read = SPI_transfer(0xFF)) != 0xFF) { 
                *token = 0xFF; 
                break; 
            }
        }

        // if data accepted
        switch (read & 0x1F){
            case 0x05:
                // set token to data accepted
                *token = 0x05;

                // wait for write to finish (timeout = 250ms)
                readAttempts = 0;
                while(1){
                    ++readAttempts;
                    read = SPI_transfer(0xFF);
                    
                    if (read != 0x00){
                        break;
                    }
                    if(readAttempts == SD_MAX_WRITE_ATTEMPTS) { 
                        *token = 0x00; 
                        break; 
                    }
                    _delay_ms(10);
                }
                break;
            case 0x0B:
                *token = 0x0B;
                break;
            case 0x0D:
                *token = 0x0D;
                break;
        }        
    }

    // deassert chip select
    SPI_transfer(0xFF);
    CS_DISABLE();
    SPI_transfer(0xFF);

    return res1;
}


void sd_read_settings(){
    SD_readSingleBlock(SIGNAL_DATA_SECTOR_NUM, sd_buffer, &token);
}
void sd_write_settings(){
    SD_writeSingleBlock(SIGNAL_DATA_SECTOR_NUM, sd_buffer, &token);
}


void sd_reset_write_cursor(){
    sd_cursor.byte_num = 0;
    sd_cursor.value_num = 0;
    sd_cursor.sector_num = SIGNAL_VALUES_START_SECTOR_NUM;
}

void sd_write_next_byte(uint8_t data){    
    sd_buffer[sd_cursor.byte_num] = data;
    
    ++sd_cursor.byte_num;    
    if (sd_cursor.byte_num >= SD_BLOCK_LEN){
#if USE_SD_CARD
        SD_writeSingleBlock(sd_cursor.sector_num, sd_buffer, &token);
        
#if TRACE_BLOCK_READ_WRITE
        IndicateSDWrite();        
#endif
        
        ++sd_cursor.sector_num;
        sd_cursor.byte_num = 0;      
#else
        next_state = WAIT_FOR_SCAN;
#endif
    }
}

void sd_write_left_bytes_if_need(){
    if (sd_cursor.byte_num >= SD_BLOCK_LEN) return;
    
    SD_writeSingleBlock(sd_cursor.sector_num, sd_buffer, &token);
        
#if TRACE_BLOCK_READ_WRITE
    IndicateSDWrite();        
#endif 
}

void sd_write_signal_data(){    
    SD_readSingleBlock(SIGNAL_DATA_SECTOR_NUM, sd_buffer, &token); 
    channels_mask = sd_buffer[CHANNELS_MASK_BYTE]; 
    
    uint16_t i;
    for (i = 0; i < SD_BLOCK_LEN; ++i) sd_buffer[i] = 0;
    
    sd_buffer[0] = (uint8_t)(sd_cursor.value_num >> 0);
    sd_buffer[1] = (uint8_t)(sd_cursor.value_num >> 8);
    sd_buffer[2] = (uint8_t)(sd_cursor.value_num >> 16);
    sd_buffer[3] = (uint8_t)(sd_cursor.value_num >> 24);
    sd_buffer[CHANNELS_MASK_BYTE] = channels_mask;
    
    SD_writeSingleBlock(SIGNAL_DATA_SECTOR_NUM, sd_buffer, &token);      
        
#if TRACE_BLOCK_READ_WRITE
    IndicateSDWrite();        
#endif
}


void sd_read_signal_data(){
#if USE_SD_CARD
    SD_readSingleBlock(SIGNAL_DATA_SECTOR_NUM, sd_buffer, &token);
    
    signal_length = 0;
    signal_length |= ( ((uint32_t)sd_buffer[0]) << 0 );
    signal_length |= ( ((uint32_t)sd_buffer[1]) << 8 );
    signal_length |= ( ((uint32_t)sd_buffer[2]) << 16 );
    signal_length |= ( ((uint32_t)sd_buffer[3]) << 24 );
    
    channels_mask = sd_buffer[CHANNELS_MASK_BYTE];
#endif
}

void sd_start_read_signal_values(){
#if USE_SD_CARD
    sd_cursor.byte_num = 0;
    sd_cursor.value_num = 0;
    sd_cursor.sector_num = SIGNAL_VALUES_START_SECTOR_NUM;
    
    SD_readSingleBlock(sd_cursor.sector_num, sd_buffer, &token);
#endif
}

uint8_t sd_read_next_byte(){     
    uint8_t data = sd_buffer[sd_cursor.byte_num];
    
    ++sd_cursor.byte_num;       
    if (sd_cursor.byte_num >= SD_BLOCK_LEN){
#if USE_SD_CARD        
        ++sd_cursor.sector_num;
        SD_readSingleBlock(sd_cursor.sector_num, sd_buffer, &token);
#endif
        sd_cursor.byte_num = 0;  
    }
    
    return data;
}



void scan_while_btn_pressed(){ 
    sd_read_signal_data();
    sd_reset_write_cursor();
    
    while (next_state == SCANNING){  
        uint8_t channel_num;
        for (channel_num = 0; channel_num < 8; ++channel_num){
            if (channels_mask & (1 << channel_num)){
                uint16_t value = adc_convert(channel_num);
                sd_write_next_byte((uint8_t)(value));
                sd_write_next_byte((uint8_t)(value >> 8));
            } else {
                sd_write_next_byte(0);
                sd_write_next_byte(0);
            }
            state_leds(); 
        }
        
//        if (sd_cursor.sector_num == 1) Mark();
//            
//        next_state = WAIT_FOR_SCAN;
        
        ++sd_cursor.value_num;
            
        //250 Hz ~ 4ms
        _delay_ms(4);
    }    
    
#if USE_SD_CARD
    sd_write_left_bytes_if_need();
    sd_write_signal_data();
#else
    signal_length = sd_cursor.value_num;
#endif
}