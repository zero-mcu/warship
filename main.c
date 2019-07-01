#if defined STM32F1
# include <stm32f1xx.h>
#elif defined STM32F2
# include <stm32f2xx_hal.h>
#elif defined STM32F4
# include <stm32f4xx_hal.h>
#endif

int main(void)
{
    for (;;);
    return 0;
}