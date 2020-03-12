/* 
 * File:   main.c
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 2:00
 */

#include "main.h"

//#define M_S = 100;//16384;

FATFS fs;          /* Work area (file system object) for the volume */
BYTE buff[100];     /* File read buffer */
UINT br;           /* File read count */
FRESULT res;       /* Petit FatFs function common result code */

static void testSD();

int main(int argc, char** argv) {
    initPins();     
    
    //testSD();
    
    while(1){
        state_pin();
        state_transit();
    }

    return (0);
}

static void testSD(){
    res = disk_initialize();
    if (res) state = CARD_ERROR;
    
    res = pf_mount(&fs);
    if (res == FR_NO_FILESYSTEM) { setUploadLedColor(GREEN); }
    pf_mount(NULL);
    while(1);
    
    if (res) state = CARD_ERROR;
    if (FatFs != NULL) setScanLedColor(GREEN); 
    else setScanLedColor(RED); 
    
    res = pf_open("signal.dat");
    if (res) state = CARD_ERROR;
    
    uint8_t i;
    for (i = 0; i < sizeof(buff); ++i) buff[i] = i;
    
    UINT bw;
    
    res = pf_write(buff, sizeof(buff), &bw);
    
//    if (bw == 10) { setUploadLedColor(RED); }
//    else { setUploadLedColor(GREEN); }
    
    if (res == FR_NOT_ENABLED){ setScanLedColor(GREEN); }
    else { setScanLedColor(RED); 
    
    setUploadLedColor(GREEN);}
    
    pf_mount(NULL);
    while(1);
  
//    else if (ds == STA_NODISK) setUploadLedColor(RED);
//    else setScanLedColor(GREEN);
  
//    pf_mount(NULL);
    if (res == FR_NO_FILE) setScanLedColor(RED);
//    else if (ds == STA_NODISK) setUploadLedColor(RED);
//    else setScanLedColor(GREEN);
    while(1);
}

void state_transit(){    
    switch (state){
        case CARD_ERROR :
            break;
        case WAIT_FOR_SCAN :       
            if (btn_mode_pressed()){ 
                state_set(WAIT_FOR_UPLOAD);
                _delay_ms(1000); 
                break;
            }
            if (btn_start_stop_pressed()){ 
                state_set(SCANNING); 
                _delay_ms(1000);
                break; }
            break;
        case SCANNING :
            //can't change mode
            if (btn_start_stop_pressed()){ 
                state_set(WAIT_FOR_SCAN); 
                _delay_ms(1000); 
                break; 
            }
            break;
        case WAIT_FOR_UPLOAD :
            if (btn_mode_pressed()){ 
                state_set(WAIT_FOR_SCAN); 
                _delay_ms(1000); 
                break; 
            }
            if (btn_start_stop_pressed()){ 
                state_set(UPLOADIND); 
                initUART();    
                createSignal();
                prepareToUploading();
                _delay_ms(1000); 
                break;
            }
            break;
        case UPLOADIND :
            if (btn_mode_pressed()){ 
                state_set(WAIT_FOR_UPLOAD); 
                _delay_ms(1000); break; 
            }
            if (btn_start_stop_pressed()){ 
                state_set(WAIT_FOR_UPLOAD); 
                _delay_ms(1000);
                break; 
            }
            stateProcessing();
            break;
        case UPLOADED :
            if (btn_mode_pressed()){ 
                state_set(WAIT_FOR_SCAN); 
                _delay_ms(1000); 
                break; 
            }
            //nothing to start and stop
            break;
    }
}

