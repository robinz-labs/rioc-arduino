#ifndef LIB_RIOC_ALADDIN2560_H_
#define LIB_RIOC_ALADDIN2560_H_

// pins on aladdin 2560 board

#define PIN_UD_DI0   22
#define PIN_UD_DI1   23
#define PIN_UD_DI2   24
#define PIN_UD_DI3   25
#define PIN_UD_DI4   26
#define PIN_UD_DI5   27
#define PIN_UD_DI6   28
#define PIN_UD_DI7   29

#define PIN_UD_DI8   30
#define PIN_UD_DI9   31
#define PIN_UD_DI10  32
#define PIN_UD_DI11  33
#define PIN_UD_DI12  34
#define PIN_UD_DI13  35
#define PIN_UD_DI14  36
#define PIN_UD_DI15  37

#define PIN_UD_DO0   2
#define PIN_UD_DO1   3
#define PIN_UD_DO2   6
#define PIN_UD_DO3   7
#define PIN_UD_DO4   8
#define PIN_UD_DO5   11
#define PIN_UD_DO6   12
#define PIN_UD_DO7   13

#define PIN_UD_ADC0  54
#define PIN_UD_ADC1  55
#define PIN_UD_ADC2  56
#define PIN_UD_VREF  57

#define PORT_UD_232  1
#define PORT_UD_485  2

const int PIN_UD_DI[16] = {
    PIN_UD_DI0,  PIN_UD_DI1,  PIN_UD_DI2,  PIN_UD_DI3,
    PIN_UD_DI4,  PIN_UD_DI5,  PIN_UD_DI6,  PIN_UD_DI7,
    PIN_UD_DI8,  PIN_UD_DI9,  PIN_UD_DI10, PIN_UD_DI11,
    PIN_UD_DI12, PIN_UD_DI13, PIN_UD_DI14, PIN_UD_DI15
};

const int PIN_UD_DO[8] = {
    PIN_UD_DO0,  PIN_UD_DO1,  PIN_UD_DO2,  PIN_UD_DO3,
    PIN_UD_DO4,  PIN_UD_DO5,  PIN_UD_DO6,  PIN_UD_DO7
};

const int PIN_UD_ADC[4] = {
    PIN_UD_ADC0, PIN_UD_ADC1, PIN_UD_ADC2, PIN_UD_VREF
};

#endif // LIB_RIOC_ALADDIN2560_H_
