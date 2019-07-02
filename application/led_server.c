#include <common/zero.h>
#include <chip/gpio.h>
#include "led_server.h"


static void led_thread(void const* arg)
{
    print("led thread running ...\r\n");
    for(;;)
    {
        gpio_set_level(0, GPIO_LEVEL_HIGH);
        sys_sleep(0.5);
        gpio_set_level(0, GPIO_LEVEL_LOW);
        sys_sleep(0.5);
    }
}

void led_server_init()
{
    osThreadDef(led, led_thread, osPriorityNormal, 0, 256);
    osThreadCreate(osThread(led), NULL);
}