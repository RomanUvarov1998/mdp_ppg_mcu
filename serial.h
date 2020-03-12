/* 
 * File:   serial.h
 * Author: Рома
 *
 * Created on 6 марта 2020 г., 20:59
 */

#ifndef SERIAL_H
    #define	SERIAL_H

    #include "main.h"

    void prepareToUploading();
    void stateProcessing();
    void createSignal();
    void initUART();
    uint8_t receiveByte(void);
    void transmitByte(uint8_t data);
    void SendBuffer();
    void SendSignal();

#endif	/* SERIAL_H */

