/* 
 * File:   sd_card.h
 * Author: Рома
 *
 * Created on 18 марта 2020 г., 19:19
 */

#ifndef SD_CARD_H
    #define	SD_CARD_H

    #include "main.h"

    uint8_t initSD();
    
    #define SDCARD_RW_BLOCK_SIZE		512

    uint8_t sd_buffer[SDCARD_RW_BLOCK_SIZE];  

    volatile struct {
        uint32_t sector_num;
        uint32_t byte_num;
        uint32_t value_num;
    } sd_cursor;
    
    volatile uint32_t signal_length;

    void reset_sd_cursor();
    uint8_t sd_next_byte();
    uint8_t sd_has_next_byte();
    
    void createAndSaveSignal(uint32_t length);    
    uint8_t SDCARD_ReadBlock(uint8_t block[SDCARD_RW_BLOCK_SIZE],uint32_t start_addr);
    uint8_t SDCARD_WriteBlock(uint8_t block[SDCARD_RW_BLOCK_SIZE],uint32_t start_addr);

#endif	/* SD_CARD_H */

