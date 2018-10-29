/*
 * DIO.h
 *
 *  Created on: Oct 05, 2018
 *      Author: epffpe
 */

#ifndef DIO_H_
#define DIO_H_

/*
 *  CONFIGURATION CONSTANTS
 *
 */

#define DO_ON       (1)
#define DO_OFF      (0)

typedef enum {
    DIO_5V_OUT_STAT,
    DIO_INTERNAL_5V_SW_STATUS,
    /*
     * DIGITAL INPUTS AND OUTPUTS
     */
    DIO_IRDA_RX,
    DIO_TEMP_ALERT,
    DIO_GPI_0,
    DIO_GPI_1,
    DIO_GPI_2,
    DIO_GPI_3,
    DIO_GPI_4,
    DIO_GPI_5,

    /*
     * ANALOG
     */
    DIO_ADC_SSTRB,

    DIO_INPUTMAP_COUNT,
}DIO_InputMap;

typedef enum {
    /*
     * MCU - POWER AND JTAG
     */
    DIO_UART_DEBUG,
    DIO_SERIAL5_EN_,

    DIO_LED_D6,
    DIO_LED_D20,
    DIO_USB0EPEN,

    DIO_5V_OUT_EN,
    DIO_PWR_PERIPHERAL_EN,
    /*
     * Digital Input Outpus
     */
    DIO_GPO_0,
    DIO_GPO_1,
    DIO_GPO_2,
    DIO_GPO_3,
    DIO_GPO_4,
    DIO_GPO_5,
    DIO_IRDA_TX,

    DIO_OUTPUTMAP_COUNT,
}DIO_OutputMap;

#ifndef CFG_H


#define DIO_TASK_PRIO           7
#define DIO_TASK_DLY_TICKS      1
#define DIO_TASK_STK_SIZE       768

#define DIO_MAX_DI              DIO_INPUTMAP_COUNT
#define DIO_MAX_DI_LOGIC        0
#define DIO_MAX_DO              DIO_OUTPUTMAP_COUNT

#define DI_EDGE_EN              1

#define DO_BLINK_MODE_EN        1

#endif

#ifdef _DIO_GLOBALS
#define DIO_EXT
#else
#define DIO_EXT extern
#endif

/*
 *
 * DISCRETE INPUT CONSTANT
 *
 */

                                            /* DI MODE SELECTOR VALUES */
#define DI_MODE_LOW                 0       /* Input is forced low */
#define DI_MODE_HIGH                1       /* Input is forced high */
#define DI_MODE_DIRECT              2       /* Input is based on state of physical input */
#define DI_MODE_INV                 3       /* Input is based on the complement of the physical input */
#define DI_MODE_EDGE_LOW_GOING      4       /* Low going edge detection of input */
#define DI_MODE_EDGE_HIGH_GOING     5       /* High going edge detection of input */
#define DI_MODE_EDGE_BOTH           6       /* Both low and high going edge detection of input */
#define DI_MODE_TOGGLE_LOW_GOING    7       /* Low going edge detection of input */
#define DI_MODE_TOGGLE_HIGH_GOING   8       /* High going edge detection of input */

                                            /* DI EDGE TRIGGERING MODE SELECTOR VALUES */
#define DI_EDGE_LOW_GOING           0       /* Negative going edge */
#define DI_EDGE_HIGH_GOING          1       /* Positive going edge */
#define DI_EDGE_BOTH                2       /* Both positive and negative going */

/*
 *
 * DISCRETE OUTPUT CONSTANTS
 *
 */
                                           /* DO MODE SELECTOR VALUES */
#define DO_MODE_LOW                 0      /* Output will be low */
#define DO_MODE_HIGH                1      /* Output will be high */
#define DO_MODE_DIRECT              2      /* Output is based on state of user supplied state */
#define DO_MODE_BLINK_SYNC          3      /* Sync. Blink mode */
#define DO_MODE_BLINK_ASYNC         4      /* Async. Blink mode */

                                           /* DO BLINK MODE ENABLE SELECTOR VALUES */
#define DO_BLINK_EN                 0      /* Blink is always enabled */
#define DO_BLINK_EN_NORMAL          1      /* Blink depends on user request's state */
#define DO_BLINK_EN_INV             2      /* Blink depends on the complemented user request's state */


typedef void (*DITriggerChangeAtInputWithValueFxn)(uint8_t input, uint32_t value);
/*
 *
 * DATA TYPES
 *
 */
typedef enum {
    KEY_STATE_UP,
    KEY_STATE_DEBOUNCE,
    KEY_STATE_RPT_START_DLY,
    KEY_STATE_RPT_DLY
} DIO_DEBOUNCE_STATE;


typedef struct dio_di {                   /* DISCRETE INPUT CHANNEL DATA STRUCTURE */
    bool        DIIn;                     /* Current state of sensor input */
    uint32_t    DIVal;                    /* State of discrete input channel (or # of transitions) */
    uint32_t    DIValPrev;                    /* State of discrete input channel (or # of transitions) */
    bool        DIPrev;                   /* Previous state of DIIn for edge detection */
    bool        DIBypassEn;               /* Bypass enable switch (Bypass when TRUE) */
    uint8_t     DIModeSel;                /* Discrete input channel mode selector */
#if DI_EDGE_EN
    void        (*DITrigFnct)(void *);    /* Function to execute if edge triggered */
    void        *DITrigFnctArg;           /* arguments passed to function when edge detected */
#endif
                                          /* DEBOUNCE PARAMETERS */
    bool        DIDebounceEn;
    uint32_t    DIDownTmr;
    uint32_t    DIDebounceDlyCtr;
    uint32_t    DIDebounceDly;
    uint32_t    DIRptStartDlyCtr;
    uint32_t    DIRptStartDly;
    uint32_t    DIRptDlyCtr;
    uint32_t    DIRptDly;
    uint32_t    DIScanState;
    void        (*DIDebFnct)(void *);    /* Function to execute if edge triggered */
    void        *DIDebFnctArg;           /* arguments passed to function when edge detected */


}DIO_DI;

typedef struct dio_do {
    bool        DOOut;
    bool        DOCtrl;
    bool        DOBypass;
    bool        DOBypassEn;
    uint8_t     DOModeSel;
    uint8_t     DOBlinkEnSel;
    bool        DOInv;
#if DO_BLINK_MODE_EN
    uint32_t    DOA;
    uint32_t    DOB;
    uint32_t    DOBCtr;
#endif
}DIO_DO;

typedef struct dio_di_map {
    unsigned int     DIOPinMap;
}DIO_MAP;


#ifdef __cplusplus
extern "C"
{
#endif

/*
 *
 * GLOBAL VARIABLES
 *
 */

DIO_EXT DIO_DI      DITbl[DIO_MAX_DI + DIO_MAX_DI_LOGIC];
DIO_EXT DIO_DO      DOTbl[DIO_MAX_DO];

DIO_EXT DITriggerChangeAtInputWithValueFxn pDITrigFxn;

/*
 *
 * FUNCTION PROTOTYPES API
 *
 */

void DIO_init(void);

void        DICfgMode(uint8_t n, uint8_t mode);
uint32_t    DIGet(uint8_t n);
void        DISetBypassEn (uint8_t n, bool state);
void        DISetBypass(uint8_t n, uint32_t val);
void        DISetDebounceEn(uint8_t n, bool state);
void        DICfgDebounce (uint8_t n, uint32_t debDly, uint32_t rpStartDly, uint32_t rpDly);

#if         DI_EDGE_EN
void        DIClr(uint8_t n);
void        DICfgEdgeDetectFnct(uint8_t n, void (*fnct)(void *), void *arg);
#endif

void        DOCfgMode (uint8_t n, uint8_t mode, bool inv);
bool        DOGet (uint8_t n);
void        DOSet (uint8_t n, bool state);
void        DOSetBypassEn (uint8_t n, bool state);
void        DOSetBypass (uint8_t n, bool state);

#if         DO_BLINK_MODE_EN
void        DOCfgBlink (uint8_t n, uint8_t mode, uint32_t a, uint32_t b);
void        DOSetSyncCtrMax(uint32_t val);
#endif

void DIO_InitIO(void);
void DIRd(void);
void DOWr(void);


#ifdef __cplusplus
}
#endif

#endif /* DIO_H_ */





