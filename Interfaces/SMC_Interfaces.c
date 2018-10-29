/*
 * SMC_Interfaces.c
 *
 *  Created on: Mar 6, 2018
 *      Author: epenate
 */

#define __INTERFACES_SMC_INTERFACES_GLOBAL
#include "includes.h"

IFUART_Object ifuartObjects[IF_SERIAL_7 + 1];

const IFUART_HWAttrs ifuartHWattrs[IF_SERIAL_7 + 1] =
{
 {
  .uartIndex = Board_Serial0,
  .driverEnablePin = SMC_SERIAL0_DE,
  .receiverEnablePin = SMC_SERIAL0_RE,
  .custom = (uintptr_t) NULL,
 },
 {
  .uartIndex = Board_Serial1,
  .driverEnablePin = SMC_SERIAL1_DE,
  .receiverEnablePin = SMC_SERIAL1_RE,
  .custom = (uintptr_t) NULL,
 },
 {
  .uartIndex = Board_Serial2,
  .driverEnablePin = SMC_SERIAL2_DE,
  .receiverEnablePin = SMC_SERIAL2_RE,
  .custom = (uintptr_t) NULL,
 },
 {
  .uartIndex = Board_Serial3,
  .driverEnablePin = SMC_SERIAL3_DE,
  .receiverEnablePin = SMC_SERIAL3_RE,
  .custom = (uintptr_t) NULL,
 },
 {
  .uartIndex = Board_Serial4,
  .driverEnablePin = SMC_SERIAL4_DE,
  .receiverEnablePin = SMC_SERIAL4_RE,
  .custom = (uintptr_t) NULL,
 },
 {
  .uartIndex = Board_Serial5,
  .driverEnablePin = NULL,
  .receiverEnablePin = NULL,
  .custom = (uintptr_t) NULL,
 },
 {
  .uartIndex = Board_Serial6,
  .driverEnablePin = SMC_SERIAL6_DE,
  .receiverEnablePin = SMC_SERIAL6_RE,
  .custom = (uintptr_t) NULL,
 },
 {
  .uartIndex = Board_Serial7,
  .driverEnablePin = NULL,
  .receiverEnablePin = NULL,
  .custom = (uintptr_t) NULL,
 },
};

const IF_Config IF_config[] =
{
 {
  .fxnTablePtr = &IFUART_fxnTable,
  .object = &ifuartObjects[0],
  .hwAttrs = (void *)&ifuartHWattrs[0],
 },
 {
  .fxnTablePtr = &IFUART_fxnTable,
  .object = &ifuartObjects[1],
  .hwAttrs = (void *)&ifuartHWattrs[1],
 },
 {
  .fxnTablePtr = &IFUART_fxnTable,
  .object = &ifuartObjects[2],
  .hwAttrs = (void *)&ifuartHWattrs[2],
 },
 {
  .fxnTablePtr = &IFUART_fxnTable,
  .object = &ifuartObjects[3],
  .hwAttrs = (void *)&ifuartHWattrs[3],
 },
 {
  .fxnTablePtr = &IFUART_fxnTable,
  .object = &ifuartObjects[4],
  .hwAttrs = (void *)&ifuartHWattrs[4],
 },
 {
  .fxnTablePtr = &IFUART_fxnTable,
  .object = &ifuartObjects[5],
  .hwAttrs = (void *)&ifuartHWattrs[5],
 },
 {
  .fxnTablePtr = &IFUART_fxnTable,
  .object = &ifuartObjects[6],
  .hwAttrs = (void *)&ifuartHWattrs[6],
 },
 {
  .fxnTablePtr = &IFUART_fxnTable,
  .object = &ifuartObjects[7],
  .hwAttrs = (void *)&ifuartHWattrs[7],
 },
 {NULL, NULL, NULL},
};
