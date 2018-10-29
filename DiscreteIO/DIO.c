/*
 * DIO.c
 *
 *  Created on: Oct 05, 2018
 *      Author: epffpe
 */


/*
 *
 * INCLUDE FILES
 *
 */

#define _DIO_GLOBALS
#include "includes.h"


/*
 *
 * LOCAL VARIABLES
 *
 */

#if DO_BLINK_MODE_EN
static uint32_t         DOSyncCtr;
static uint32_t         DOSyncCtrMax;
#endif


Task_Struct DIOTaskStruct;
Char DIOTaskStk[DIO_TASK_STK_SIZE];

/*
 *
 * LOCAL FUNCTION PROTOTYPES
 *
 */

static void DIIsTrig(DIO_DI *pdi);

static void DIUpdate(void);

Void DIOTask(UArg arg0, UArg arg1);

static bool DOIsBlinkEn(DIO_DO *pdo);

void DOSetSyncCtrMax(uint32_t val);

static void DOUpdate(void);

/*
 *
 * CONFIGURE DISCRETE INPUT EDGE DETECTION
 * DESCRIPTION: This function is used to configure the edge detection capability of the discrete input
 * channel.
 * ARGUMENTS
 *
 * RETURNS
 */

#if DI_EDGE_EN
void DICfgEdgeDetectFnct(uint8_t n, void (*fnct)(void *), void *arg)
{
    if (n < DIO_MAX_DI) {
        OS_ENTER_CRITICAL();
        DITbl[n].DITrigFnct = fnct;
        DITbl[n].DITrigFnctArg = arg;
        OS_EXIT_CRITICAL();
    }
}
#endif

/*
 * DI_MODE_LOW
 * DI_MODE_HIGH
 * DI_MODE_DIRECT
 * DI_MODE_INV
 * DI_MODE_EDGE_LOW_GOING
 * DI_MODE_EDGE_HIGH_GOING
 * DI_MODE_EDGE_BOTH
 * DI_MODE_TOGGLE_LOW_GOING
 * DI_MODE_TOGGLE_HIGH_GOING
 *
 */
void DICfgMode(uint8_t n, uint8_t mode)
{
    if (n < DIO_MAX_DI){
        OS_ENTER_CRITICAL();
        DITbl[n].DIModeSel = mode;
        OS_EXIT_CRITICAL();
    }
}

/*
 *
 *
 *
 */

#if DI_EDGE_EN
void DIClr(uint8_t n)
{
    DIO_DI *pdi;

    if (n < DIO_MAX_DI) {
        pdi = &DITbl[n];
        OS_ENTER_CRITICAL();
        if (    pdi->DIModeSel == DI_MODE_EDGE_LOW_GOING ||
                pdi->DIModeSel == DI_MODE_EDGE_HIGH_GOING ||
                pdi->DIModeSel == DI_MODE_EDGE_BOTH) {
            pdi->DIVal = 0;
        }
        OS_EXIT_CRITICAL();
    }
}
#endif

/*
 *
 *
 *
 */

uint32_t DIGet(uint8_t n)
{
    uint32_t val;

    if (n < DIO_MAX_DI) {
        OS_ENTER_CRITICAL();
        val = DITbl[n].DIVal;
        OS_EXIT_CRITICAL();
        return(val);
    }else{
        return (0);
    }
}

/*
 *
 * This function is called to detect an edge (low-going, high-going or both) on the selected
 * discrete input.
 *
 */

#ifdef DI_EDGE_EN
static void DIIsTrig(DIO_DI *pdi)
{
    bool trig;

    trig = false;
    switch (pdi->DIModeSel) {
    case DI_MODE_EDGE_LOW_GOING:
        if (pdi->DIPrev == 1 && pdi->DIIn == 0) {
            trig = true;
        }
        break;
    case DI_MODE_EDGE_HIGH_GOING:
        if (pdi->DIPrev == 0 && pdi->DIIn == 1) {
            trig = true;
        }
        break;
    case DI_MODE_EDGE_BOTH:
        if (    (pdi->DIPrev == 0 && pdi->DIIn == 1) ||
                (pdi->DIPrev == 1 && pdi->DIIn == 0) ) {
            trig = true;
        }
        break;
    }
    if (trig == true){
        if (pdi->DITrigFnct != NULL) {
            (*pdi->DITrigFnct)(pdi->DITrigFnctArg);
        }
        if (pdi->DIVal < 0xFFFFFFFF) {
            pdi->DIVal++;
        }
    }
    pdi->DIPrev = pdi->DIIn;
}
#endif


void DITriggerChangeAtInputWithValue(uint8_t input, uint32_t value)
{
    if (pDITrigFxn) {
        pDITrigFxn(input, value);
    }
}

/*
 *
 * This function processes all of the discrete input channels.
 *
 */

static void DIUpdate(void) {
    uint8_t i;
    DIO_DI *pdi;

    pdi = &DITbl[0];
    for (i = 0; i < DIO_MAX_DI; i++) {
        if (pdi->DIBypassEn == false) {
            switch (pdi->DIModeSel) {
            case DI_MODE_LOW:
                pdi->DIVal = 0;
                break;
            case DI_MODE_HIGH:
                pdi->DIVal = 1;
                break;
            case DI_MODE_DIRECT:
                pdi->DIVal = (uint32_t)pdi->DIIn;
                break;
            case DI_MODE_INV:
                pdi->DIVal = (uint32_t)(pdi->DIIn ? 0 : 1);
                break;
#ifdef DI_EDGE_EN
            case DI_MODE_EDGE_LOW_GOING:
            case DI_MODE_EDGE_HIGH_GOING:
            case DI_MODE_EDGE_BOTH:
                DIIsTrig(pdi);
                break;
#endif
            case DI_MODE_TOGGLE_LOW_GOING:
                if (pdi->DIPrev == 1 && pdi->DIIn == 0) {
                    pdi->DIVal = pdi->DIVal ? 0 : 1;
                }
                pdi->DIPrev = pdi->DIIn;
                break;
            case DI_MODE_TOGGLE_HIGH_GOING:
                if (pdi->DIPrev == 0 && pdi->DIIn == 1) {
                    pdi->DIVal = pdi->DIVal ? 0 : 1;
                }
                pdi->DIPrev = pdi->DIIn;
                break;
            default:
                break;
            }
        }
        if (pdi->DIValPrev != pdi->DIVal) {
            DITriggerChangeAtInputWithValue(i, pdi->DIVal);
        }
        pdi->DIValPrev = pdi->DIVal;
        pdi++;
    }
}

/*
 *
 * DISCRETE I/O MANAGER INITIALIZATION
 *
 */

void DIO_init(void)
{
    uint32_t i;
    DIO_DI *pdi;
    DIO_DO *pdo;

    Task_Handle taskHandle;
    Task_Params taskParams;
    Error_Block eb;

    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    pDITrigFxn = NULL;

    pdi = &DITbl[0];
    for (i = 0; i < DIO_MAX_DI + DIO_MAX_DI_LOGIC; i++) {
        pdi->DIVal          = 0;
        pdi->DIBypassEn     = false;
        pdi->DIModeSel      = DI_MODE_DIRECT;
#if DI_EDGE_EN
        pdi->DITrigFnct     = (void *)0;
        pdi->DITrigFnctArg  = (void *)0;
#endif
        pdi->DIDebounceEn   = false;
        pdi->DIDownTmr      = 0;
        pdi->DIDebounceDly  = 3;
        pdi->DIDebounceDlyCtr = 0;
        pdi->DIRptDlyCtr    = 0;
        pdi->DIRptStartDlyCtr = 0;
        pdi->DIRptDly       = 10;
        pdi->DIRptStartDly  = 50;
        pdi->DIScanState    = KEY_STATE_UP;
        pdi->DIDebFnct      = (void *)0;
        pdi->DIDebFnctArg   = (void *)0;
        pdi++;
    }

    /* Set the inputs comming from BLE to bypass */
    for (i = DIO_MAX_DI; i < DIO_MAX_DI + DIO_MAX_DI_LOGIC; i++) {
        pdi = &DITbl[i];
        pdi->DIBypassEn     = true;
    }

    pdo = &DOTbl[0];
    for (i = 0; i < DIO_MAX_DO; i++) {
        pdo->DOOut          = 0;
        pdo->DOBypassEn     = false;
        pdo->DOModeSel      = DO_MODE_DIRECT;
        pdo->DOInv          = false;
#if DO_BLINK_MODE_EN
      pdo->DOBlinkEnSel     = DO_BLINK_EN_NORMAL;
      pdo->DOA              = 1;
      pdo->DOB              = 2;
      pdo->DOBCtr           = 2;
#endif
      pdo++;
    }
#if DO_BLINK_MODE_EN
    DOSetSyncCtrMax(72);
#endif
    DIO_InitIO();

    /*
     *  Create the Task that farms out incoming TCP connections.
     *  arg0 will be the port that this task listens to.
     */
    Task_Params_init(&taskParams);
    taskParams.stackSize = DIO_TASK_STK_SIZE;
    taskParams.priority = DIO_TASK_PRIO;
    taskParams.arg0 = (UArg)0;
    taskParams.stack = DIOTaskStk;
    taskHandle = Task_create((Task_FuncPtr)DIOTask, &taskParams, &eb);
    if (taskHandle == NULL) {
        System_printf("Failed to create DIOTask Task\n");
    }

    System_flush();
}


/*
 *
 *
 *
 */


Void DIOTask(UArg arg0, UArg arg1)
{
    arg0 = arg0;
    arg1 = arg1;

    for(;;) {
        Task_sleep(DIO_TASK_DLY_TICKS);
        DIRd();
        DIUpdate();
        DOUpdate();
        DOWr();
    }
}

/*
 *
 * SET THE STATE OF THE BYPASSED SENSOR
 * This function is used to set the state of the bypassed sensor. This function is used to
 * simulate the presence of the sensor. This function is only valid if the bypass 'switch'
 * is open.
 *
 * val is the state of the bypassed sensor:
 *  0   indicates a negated sensor
 *  1   indicates an asserted sensor
 *  >0  indicates the number of edges detected in edge mode
 */

void DISetBypass(uint8_t n, uint32_t val)
{
    if (n < DIO_MAX_DI) {
        OS_ENTER_CRITICAL();
        if (DITbl[n].DIBypassEn == true) {
            DITbl[n].DIVal = val;
        }
        OS_EXIT_CRITICAL();
    }
}



/*
 *
 *
 */

void DISetBypassEn (uint8_t n, bool state)
{
    if (n < DIO_MAX_DI) {
        OS_ENTER_CRITICAL();
        DITbl[n].DIBypassEn = state;
        OS_EXIT_CRITICAL();
    }
}

void DISetDebounceEn(uint8_t n, bool state)
{
    if (n < DIO_MAX_DI) {
        OS_ENTER_CRITICAL();
        DITbl[n].DIDebounceEn = state;
        OS_EXIT_CRITICAL();
    }
}

void DICfgDebounce (uint8_t n, uint32_t debDly, uint32_t rpStartDly, uint32_t rpDly)
{
    debDly          /= DIO_TASK_DLY_TICKS;
    rpStartDly      /= DIO_TASK_DLY_TICKS;
    rpDly           /= DIO_TASK_DLY_TICKS;

    if (n < DIO_MAX_DI) {
        OS_ENTER_CRITICAL();
        DITbl[n].DIDebounceDly  = debDly;
        DITbl[n].DIRptStartDly  = rpStartDly;
        DITbl[n].DIRptDly       = rpDly;
        OS_EXIT_CRITICAL();
    }
}


/*
 *
 * CONFIGURE THE DISCRETE OUTPUT BLINK MODE
 * This function is used to configure the blink mode of the discrete output channel.
 *  mode is the desired blink mode:
 *      DO_BLINK_EN
 *      DO_BLINK_EN_NORMAL
 *      DO_BLINK_EN_INV
 *  a   is the number of ticks ON
 *  b   is the number of ticks for the period (in DO_MODE_BLINK_ASYNC mode)
 */

#if DO_BLINK_MODE_EN
void DOCfgBlink (uint8_t n, uint8_t mode, uint32_t a, uint32_t b)
{
    DIO_DO *pdo;

    if (n < DIO_MAX_DO) {
        pdo             = &DOTbl[n];
        a              /= DIO_TASK_DLY_TICKS;
        b              /= DIO_TASK_DLY_TICKS;
        OS_ENTER_CRITICAL();
        pdo->DOBlinkEnSel = mode;
        pdo->DOA        = a;
        pdo->DOB        = b;
        OS_EXIT_CRITICAL();
    }
}
#endif

/*
 *
 * CONFIGURE DISCRETE OUTPUT MODE
 * This function is used to configure the mode of a discrete output channel.
 *  mode is the desired mode and can be:
 *      DO_MODE_LOW
 *      DO_MODE_HIGH
 *      DO_MODE_DIRECT
 *      DO_MODE_BLINK_SYNC
 *      DO_MODE_BLINK_ASYNC
 *  inv indicates whether the output will be inverted
 *      true forces teh output to be inverted
 *      false does not cause any inversion
 *
 */

void DOCfgMode (uint8_t n, uint8_t mode, bool inv)
{
    if (n < DIO_MAX_DO) {
        OS_ENTER_CRITICAL();
        DOTbl[n].DOModeSel = mode;
        DOTbl[n].DOInv = inv;
        OS_EXIT_CRITICAL();
    }
}

/*
 *
 * GET THE STATE OF THE DISCRETE OUTPUT
 * returns
 *  true if the output is asserted
 *  false if teh output is negated
 *
 */
bool DOGet (uint8_t n)
{
    bool out;

    if (n < DIO_MAX_DO) {
        OS_ENTER_CRITICAL();
        out = DOTbl[n].DOOut;
        OS_EXIT_CRITICAL();
        return (out);
    } else {
        return false;
    }
}

/*
 *
 * SEE IF BLINK IS ENABLED
 *  pdo is a pointer to the discrete output data structure
 * returns
 *  true if blinking is enabled
 *  false otherwise
 *
 */

#if DO_BLINK_MODE_EN
static bool DOIsBlinkEn (DIO_DO *pdo)
{
    bool en;

    en = false;
    switch (pdo->DOBlinkEnSel) {
    case DO_BLINK_EN:
        en = true;
        break;
    case DO_BLINK_EN_NORMAL:
        en = pdo->DOBypass;
        break;
    case DO_BLINK_EN_INV:
        en = pdo->DOBypass ? false : true;
        break;
    }
    return(en);
}
#endif

/*
 *
 * SET THE STATE OF TEH DISCRETE OUTPUT
 * This function is used to set the state of the discrete output
 *  state is the desired state of the outpu:
 *      false indicates a negated output
 *      true indicates an asserted  output
 *  The actual output will be complemented if 'DIInv' is set to true
 *
 */

void DOSet (uint8_t n, bool state)
{
    if (n < DIO_MAX_DO) {
        OS_ENTER_CRITICAL();
        DOTbl[n].DOCtrl = state;
        OS_EXIT_CRITICAL();
    }
}


/*
 *
 * SET THE STATE OF THE BYPASSED OUTPUT
 * This function is used to set the state of the bypassed output. This function is used to
 * overrride (or bypass) the application software and allow the output to be controlled
 * directly. This function is only valid if the bypass switch is open.
 *
 */

void DOSetBypass (uint8_t n, bool state)
{
    if (n < DIO_MAX_DO) {
        OS_ENTER_CRITICAL();
        if (DOTbl[n].DOBypassEn == true) {
            DOTbl[n].DOBypass = state;
        }
        OS_EXIT_CRITICAL();
    }
}

/*
 *
 *
 *
 */

void DOSetBypassEn (uint8_t n, bool state)
{
    if (n < DIO_MAX_DO) {
        OS_ENTER_CRITICAL();
        DOTbl[n].DOBypassEn = state;
        OS_EXIT_CRITICAL();
    }
}

/*
 * SET THE MAXIMUM VALUE FOR TEH SYNCHRONOUS COUNTER
 * This function is used to set the maximum value taken by the syncronous counter which is
 * used in the synchronous blink mode.
 *
 */
#if DO_BLINK_MODE_EN
void DOSetSyncCtrMax(uint32_t val)
{
    OS_ENTER_CRITICAL();
    DOSyncCtrMax = val / DIO_TASK_DLY_TICKS;
    OS_EXIT_CRITICAL();
}
#endif

/*
 *
 *  UPDATE DISCRETE OUT CHANNELS
 *  This function is called to process all of the discrete output channels.
 *
 */

static void DOUpdate (void)
{
    uint8_t i;
    bool out;
    DIO_DO *pdo;

    pdo = &DOTbl[0];
    for (i = 0; i < DIO_MAX_DO; i++) {
        if (pdo->DOBypassEn == false) {
            pdo->DOBypass = pdo->DOCtrl;
        }

        out = false;
        switch (pdo->DOModeSel) {
        case DO_MODE_LOW:
            break;
        case DO_MODE_HIGH:
            out = true;
            break;
        case DO_MODE_DIRECT:
            out = pdo->DOBypass;
            break;
#if DO_BLINK_MODE_EN
        case DO_MODE_BLINK_SYNC:
            if (DOIsBlinkEn(pdo)) {
                if (pdo->DOA >= DOSyncCtr) {
                    out = true;
                }
            }
            break;
        case DO_MODE_BLINK_ASYNC:
            if (DOIsBlinkEn(pdo)) {
                if (pdo->DOA >= pdo->DOBCtr) {
                    out = true;
                }
            }
            if (pdo->DOBCtr < pdo->DOB) {
                pdo->DOBCtr++;
            } else {
                pdo->DOBCtr = 0;
            }
            break;
#endif
        }
        if (pdo->DOInv == true) {
            pdo->DOOut = out ? false : true;
        } else {
            pdo->DOOut = out;
        }
        pdo++;
    }
#if DO_BLINK_MODE_EN
    if (DOSyncCtr < DOSyncCtrMax) {
        DOSyncCtr++;
    } else {
        DOSyncCtr = 0;
    }
#endif
}

#ifndef CFG_C
/*
 *
 * INITIALIZE PHYSICAL I/OS
 * This function si by DIOInit() to initialize the physical I/O used by the DIO driver
 */

void DIO_InitIO (void)
{

}


/*
 *
 *  PHYSICAL INTERFACE
 *
 */
const DIO_MAP DIMapTbl[DIO_MAX_DI] =
{
     /*
      * POWER
      */
     SMC_5V_OUT_STAT,
     SMC_INTERNAL_5V_SW_STATUS,
     /*
      * DIGITAL INPUTS AND OUTPUTS
      */
     SMC_IRDA_RX,
     SMC_TEMP_ALERT,
     SMC_GPI_0,
     SMC_GPI_1,
     SMC_GPI_2,
     SMC_GPI_3,
     SMC_GPI_4,
     SMC_GPI_5,

     /*
      * ANALOG
      */
     SMC_ADC_SSTRB,

};

const DIO_MAP DOMapTbl[DIO_MAX_DO] =
{
     /*
      * MCU - POWER AND JTAG
      */
     SMC_UART_DEBUG,
     SMC_SERIAL5_EN_,

     SMC_LED_D6,
     SMC_LED_D20,
     SMC_USB0EPEN,

     SMC_5V_OUT_EN,
     SMC_PWR_PERIPHERAL_EN,
     /*
      * Digital Input Outpus
      */
     SMC_GPO_0,
     SMC_GPO_1,
     SMC_GPO_2,
     SMC_GPO_3,
     SMC_GPO_4,
     SMC_GPO_5,
     SMC_IRDA_TX,
};


/*
 *
 *  SEE IF KEY PRESSED
 *  returns
 *      true if a key is pressed
 *      false if a key is not preset
 *
 */

static bool DIIsKeyDown(uint8_t n)
{
    unsigned int pinVal;
    bool val;
    val = false;
    if (n < DIO_MAX_DI) {
        pinVal = GPIO_read(DIMapTbl[n].DIOPinMap);//CLUReadInputAtAddress(n);
        val = (bool)(pinVal ? 0 : 1);
        if (val) {
            DITbl[n].DIDownTmr++;
        }
    }
    return val;
}


/*
 *
 *  READ PHYSICAL INPUTS
 *  This function is called to read and map all of the physical inputs used for discrete
 *  inputs and map these inputs to their appropiate discrete input data structure
 */

void DIRd(void)
{
    DIO_DI *pdi;
    uint8_t i;
//    unsigned int in;

    for (i = 0; i < DIO_MAX_DI; i++) {
        pdi = &DITbl[i];
        if (pdi->DIDebounceEn) {
            switch(pdi->DIScanState) {
            case KEY_STATE_UP:
                pdi->DIIn = 1;
                if (DIIsKeyDown(i)) {
                    pdi->DIScanState = KEY_STATE_DEBOUNCE;
                    pdi->DIDownTmr = 0;
                    pdi->DIDebounceDlyCtr = pdi->DIDebounceDly;
                }
                break;
            case KEY_STATE_DEBOUNCE:
                if (DIIsKeyDown(i)) {
                    if (pdi->DIDebounceDlyCtr > 0) {
                        pdi->DIDebounceDlyCtr--;
                        if (pdi->DIDebounceDlyCtr == 0){
                            pdi->DIIn = 0;
                            if (pdi->DIDebFnct != NULL) {
                                (*pdi->DIDebFnct)(pdi->DIDebFnctArg);
                            }
                            pdi->DIRptStartDlyCtr = pdi->DIRptStartDly;
                            pdi->DIScanState = KEY_STATE_RPT_START_DLY;
                        }
                    }
                } else {
                    pdi->DIScanState = KEY_STATE_UP;
                }
                break;
            case KEY_STATE_RPT_START_DLY:
                if (DIIsKeyDown(i)) {
                    if (pdi->DIRptStartDlyCtr > 0) {
                        pdi->DIRptStartDlyCtr--;
                        if (pdi->DIRptStartDlyCtr == 0) {
                            pdi->DIIn = 0;
                            if (pdi->DIDebFnct != NULL) {
                                (*pdi->DIDebFnct)(pdi->DIDebFnctArg);
                            }
                            pdi->DIRptDlyCtr = pdi->DIRptDly;
                            pdi->DIScanState = KEY_STATE_RPT_DLY;
                        }
                    }
                } else {
                    pdi->DIScanState = KEY_STATE_DEBOUNCE;
                }
                break;
            case KEY_STATE_RPT_DLY:
                if (DIIsKeyDown(i)) {
                    if (pdi->DIRptDlyCtr > 0) {
                        pdi->DIRptDlyCtr--;
                        if (pdi->DIRptDlyCtr == 0) {
                            pdi->DIIn = 0;
                            if (pdi->DIDebFnct != NULL) {
                                (*pdi->DIDebFnct)(pdi->DIDebFnctArg);
                            }
                            pdi->DIRptDlyCtr = pdi->DIRptDly;
                        }
                    }
                } else {
                    pdi->DIScanState = KEY_STATE_DEBOUNCE;
                }
                break;
            }
        } else {
            pdi->DIIn = GPIO_read(DIMapTbl[i].DIOPinMap);
        }
    }
}

/*
 *
 * UDATE PHYSICAL OUTPUTS
 *
 */

void DOWr(void)
{
    DIO_DO *pdo;
    uint8_t i;

    for (i = 0; i<DIO_MAX_DO; i++){
        pdo = &DOTbl[i];
        GPIO_write(DOMapTbl[i].DIOPinMap, pdo->DOOut);
    }
}
#endif
