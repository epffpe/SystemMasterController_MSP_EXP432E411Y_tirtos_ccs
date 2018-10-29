/*
 * ALTOInterface.c
 *
 *  Created on: Dec 11, 2017
 *      Author: epenate
 */


#define __ALTOINTERFACE_GLOBAL
#include "includes.h"


#define HEX2ASCII(x) (((x)>=10)? (((x)-10)+'A') : ((x)+'0'))

void vALTOFrame_Params_init( ALTO_Frame_t *tframe )
{
    int i32Index;
    tframe->classID = 0;
    tframe->operationID = 0;
    tframe->functionID = 0;
    tframe->sequence = 0;
    tframe->length = 0;
    for (i32Index = 0; i32Index < ALTO_FRAME_DATA_PAYLOAD_SIZE; i32Index++ ) {
        tframe->data[i32Index] = 0;
    }
    tframe->bcc = 0;
}

int8_t cALTOFrame_BCC_calulate( ALTO_Frame_t *tframe )
{
    int8_t i8bcc = 0;
    int8_t *pi8Bcc = (int8_t *)tframe;
    int i32Index;

    for (i32Index = 0; i32Index < sizeof(ALTO_Frame_t)-1; i32Index++) {
        i8bcc ^= *pi8Bcc++;
    }

    return i8bcc;
}

void vALTOFrame_BCC_fill( ALTO_Frame_t *tframe )
{
    int8_t i8bcc = 0;
    int8_t *pi8Bcc = (int8_t *)tframe;
    int i32Index;

    for (i32Index = 0; i32Index < sizeof(ALTO_Frame_t)-1; i32Index++) {
        i8bcc ^= *pi8Bcc++;
    }
    tframe->bcc = i8bcc;
}


int8_t cALTOFrame_BCC_check( ALTO_Frame_t *tframe )
{
    int8_t i8bcc = 0;
    int8_t *pi8Bcc = (int8_t *)tframe;
    int i32Index;

    for (i32Index = 0; i32Index < sizeof(ALTO_Frame_t); i32Index++) {
        i8bcc ^= *pi8Bcc++;
    }
    return i8bcc;
}

void vALTOFrame_create_ASCII(char *pi8Buffer70, ALTO_Frame_t *tframe)
{
    int i32Index;
    char *pi8Frame = (char *)tframe;
    char *pi8Buff;
    char i8temp;
    /*
     * Prefix
     */
    pi8Buffer70[0] = 'A';
    pi8Buffer70[1] = 'A';
    pi8Buffer70[2] = '5';
    pi8Buffer70[3] = '5';


    pi8Buff = &pi8Buffer70[4];
    for (i32Index = 0; i32Index < sizeof(ALTO_Frame_t); i32Index++) {
        i8temp = *pi8Frame++;
        *pi8Buff++ = HEX2ASCII( (i8temp >> 4) & 0x0F);
        *pi8Buff++ = HEX2ASCII( i8temp & 0x0F);
    }
    /*
     * CR/LF
     */
    pi8Buffer70[68] = '\r';
    pi8Buffer70[69] = '\n';
}

static uint8_t cALTOFrame_ASCII_to_char(char ascii)
{
    uint8_t ui8retValue;
    if (('0' <= ascii) && (ascii <= '9')) {
        ui8retValue = ascii - '0';
    }else if (('A' <= ascii) && (ascii <= 'F')) {
        ui8retValue = ascii - 'A' + 0x0A;
    }else if (('a' <= ascii) && (ascii <= 'f')) {
        ui8retValue = ascii - 'a' + 0x0A;
    }
    return ui8retValue;
}

int xALTOFrame_convert_ASCII_to_binary(char *pi8Buffer64, ALTO_Frame_t *tframe)
{
    int i32retValue = 0;
    int i;
    uint8_t *pui8Frame32 = (uint8_t *)tframe;
    uint8_t lowNibble, highNibble, tempByte;

    for (i = 0; i < 32; i++) {
        lowNibble = cALTOFrame_ASCII_to_char(pi8Buffer64[2*i + 1]);
        highNibble = cALTOFrame_ASCII_to_char(pi8Buffer64[2*i]);
        tempByte = lowNibble | (highNibble << 4);
        pui8Frame32[i] = tempByte;
    }

    return i32retValue;
}

void vALTOFrame_ORTConvertDataFromBigEndianToLittleEndian(uint8_t *pui8Data, uint8_t ui8Length)
{
    uint16_t *pui16Data, ui16TempData;
    uint8_t ui8ORTDataLength, ui8Index;

    ui8ORTDataLength = ui8Length / 2;

    pui16Data = (uint16_t *)pui8Data;

    for (ui8Index = 0; ui8Index < ui8ORTDataLength; ui8Index++) {
        ui16TempData = *pui16Data;
        *pui16Data++ = (ui16TempData << 8 ) | (ui16TempData >> 8 );
    }
}

