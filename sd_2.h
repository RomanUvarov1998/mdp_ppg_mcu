/* 
 * File:   sd_2.h
 * Author: Рома
 *
 * Created on 6 апреля 2020 г., 0:07
 */

#ifndef SD_2_H
#define	SD_2_H

    #include "main.h"

    typedef uint8_t	DSTATUS;
    /* Disk Status Bits (DSTATUS) */
    #define STA_NOINIT		0x01	/* Drive not initialized */
    #define STA_NODISK		0x02	/* No medium in the drive */
    #define STA_PROTECT		0x04	/* Write protected */
    static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

    #define SIGNAL_DATA_SECTOR_NUM 0
    #define SIGNAL_VALUES_START_SECTOR_NUM (SIGNAL_DATA_SECTOR_NUM + 1)

    #define SD_BLOCK_LEN            512
    volatile uint8_t sd_buffer[SD_BLOCK_LEN];
    struct {
        volatile uint32_t byte_num;
        volatile uint32_t value_num;
        volatile uint32_t sector_num;
    } sd_cursor;
    volatile uint32_t signal_length;
    volatile uint8_t channels_mask;
    
    #define CHANNELS_MASK_BYTE 5
    #define MAX_CHANNEL_NUM 5
    
    DSTATUS mmc_disk_initialize (void);
    
    /* Results of Disk Functions */
    typedef enum {
        RES_OK = 0,		/* 0: Successful */
        RES_ERROR,		/* 1: R/W Error */
        RES_WRPRT,		/* 2: Write Protected */
        RES_NOTRDY,		/* 3: Not Ready */
        RES_PARERR		/* 4: Invalid Parameter */
    } DRESULT;
    DRESULT mmc_disk_read (
        uint8_t *buff,			/* Pointer to the data buffer to store read data */
        uint32_t sector,		/* Start sector number (LBA) */
        uint16_t count			/* Sector count (1..128) */
    );
    DRESULT mmc_disk_write (
        const uint8_t *buff,	/* Pointer to the data to be written */
        uint32_t sector,		/* Start sector number (LBA) */
        uint16_t count			/* Sector count (1..128) */
    );
    
    void sd_read_settings();
    void sd_write_settings();
    
    void sd_reset_write_cursor();
    void sd_write_next_byte(uint8_t data);
    void sd_write_left_bytes_if_need();
    void sd_write_signal_data();
    
    void sd_read_signal_data();
    void sd_start_read_signal_values();
    uint8_t sd_read_next_byte();
    
    void scan_while_btn_pressed();
    
#endif	/* SD_2_H */

