#if defined STM32F1
# include <stm32f1xx_hal.h>
#elif defined STM32F2
# include <stm32f2xx_hal.h>
#elif defined STM32F4
# include <stm32f4xx_hal.h>
#endif
#include <common/zero.h>
#include <bus/i2c.h>
#include <bus/spi.h>
#include <bus/serial.h>
#include "driver/drv_sram.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi.h"
#include "driver/spi1.h"
#include "driver/uart.h"
#include "application/led_server.h"
#include <stm32f1xx_ll_usart.h>
//ze_u8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__((section(".sram_data")));


I2C_DECLEAR(0);
SPI_DECLEAR(0);
SPI_DECLEAR(1);
SERIAL_DECLEAR(0);

void base_init(void);

int user_stdio_putc(int ch);
int user_stdio_getc(void);

static ze_stdio_t user_stdio = {
    .put_char = user_stdio_putc,
    .get_char = user_stdio_getc,
};

int user_stdio_putc(int ch)
{
    serial_write(SERIAL(0), (const ze_u8_t*)&ch, 1);
    return ch;
}

int user_stdio_getc(void)
{
    int ch = 0;
    int ret = serial_read(SERIAL(0), (ze_u8_t*)&ch, 1);
    if (ret < 0)
        return ret;
    return ch;
}

void driver_init(void)
{

    //sram_hw_init();
    base_init();
    stm_gpio_hw_init();
    
    uart_hw_init();
    stdio_init(&user_stdio);

    print("driver init passed.\r\n");
}

void application_init(void)
{
    led_server_init();
}

int main(void)
{
    HAL_Init();
    zero_init();
    driver_init();
    application_init();
    zero_schedule();
    return 0;
}