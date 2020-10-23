/*
 * RosenBlueRayDVD.c
 *
 *  Created on: Feb 14, 2018
 *      Author: epenate
 */

#define __ROSENBLUERAYDVD_GLOBAL
#include "includes.h"


uint32_t xROSENBlueRayDVDCreateMsgFrame(tsROSENCommand *ptsCmdBuf, uint32_t command, uint32_t address)
{
    ASSERT(ptsCmdBuf != NULL);
    ASSERT(command <= 0xFF);
    ASSERT((address >= ROSSEN_BlueRayDVD_Network_Address_20 ) && (address <= ROSSEN_BlueRayDVD_Network_Address_27));

    if (ptsCmdBuf == NULL) {
        return 0;
    }
    if ((address < ROSSEN_BlueRayDVD_Network_Address_20 ) || (address > ROSSEN_BlueRayDVD_Network_Address_27)) {
        return 0;
    }
    return xROSENCreateMsgFrame(ptsCmdBuf, Rosen_Header_Single_BlueRay_DVD, command, address);
}

