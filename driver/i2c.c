#include <common/zero.h>
#include <stm32f1xx_hal.h>
#include <chip/gpio.h>
#include <bus/i2c_bit.h>
#include "i2c.h"

IMPORT_I2C(0);


#define IIC_SCL_PIN           GPIO_PIN_10
#define IIC_SCL_PORT          GPIOB
#define IIC_SCL_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define IIC_SDA_PIN           GPIO_PIN_11
#define IIC_SDA_PORT          GPIOB
#define IIC_SDA_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()


int i2c_scl_set(i2c_bit_algorithm_t* algo, int value)
{
    HAL_GPIO_WritePin(IIC_SCL_PORT, IIC_SCL_PIN, value == GPIO_LEVEL_HIGH ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return 0;
}

int i2c_sda_set(i2c_bit_algorithm_t* algo, int value)
{
    HAL_GPIO_WritePin(IIC_SDA_PORT, IIC_SDA_PIN, value == GPIO_LEVEL_HIGH ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return 0;
}

int i2c_sda_get(i2c_bit_algorithm_t* algo)
{
    if(HAL_GPIO_ReadPin(IIC_SDA_PORT, IIC_SDA_PIN) == GPIO_PIN_SET)
    {
        return GPIO_LEVEL_HIGH;
    }
    else
    {
        return GPIO_LEVEL_LOW;
    }
}

int i2c_sda_set_dir(i2c_bit_algorithm_t* algo, int value)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    GPIO_InitStruct.Pin = IIC_SDA_PIN;
    if (value == GPIO_DIR_OUTPUT)
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    else
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    
    HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
    return 0;
}


i2c_bit_algorithm_t i2c_bit_algo = {
    .scl_set = i2c_scl_set,
    .sda_set = i2c_sda_set,
    .sda_get = i2c_sda_get,
    .sda_set_dir = i2c_sda_set_dir,
    .delay_us = 4,
};

static void i2c_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    IIC_SCL_CLK_ENABLE();
    IIC_SDA_CLK_ENABLE();

    GPIO_InitStruct.Pin = IIC_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
        
    HAL_GPIO_Init(IIC_SCL_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = IIC_SDA_PIN;
    
    HAL_GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
}

void i2c_hw_init(void)
{
    i2c_gpio_init();
    
    i2c_bit_algorithm_init(&i2c_bit_algo);
    i2c_init(I2C(0), &i2c_bit_algo.algo);
}
