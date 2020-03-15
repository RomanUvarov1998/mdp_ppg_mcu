/* 
 * File:   main.c
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 2:00
 */

#include "main.h"

//#define M_S = 100;//16384;

BYTE buff[100];     /* File read buffer */
UINT br;           /* File read count */
FRESULT res;       /* Petit FatFs function common result code */
FATFS fs;          /* Work area (file system object) for the volume */

static void testSD();
static void testBtnInt();

int main(int argc, char** argv) {
    initPins();     
    initBtns();
    
    //testSD()
    
    sei();
    
    while(1){
        state_pin();
        state_transit();
    }

    return (0);
}

static void testSD(){
    pf_mount(&fs);
    while(1);
    
    res = pf_mount(&fs);
    
    if (res) state = CARD_ERROR;
//    if (FatFs != NULL) setScanLedColor(GREEN); 
//    else setScanLedColor(RED); 
    
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
            break;
        case SCANNING :
            break;
        case WAIT_FOR_UPLOAD :
            break;
        case UPLOADIND :
            stateProcessing();
            break;
        case UPLOADED :
            break;
    }
}

