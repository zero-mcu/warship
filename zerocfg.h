#ifndef __ZEROCFG_H__
#define __ZEROCFG_H__

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
    #include <stm32f1xx_hal.h>
    extern uint32_t SystemCoreClock;
#endif

#define CPU_CLOCK_HZ            (SystemCoreClock)

#endif