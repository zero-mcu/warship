#include <common/zero.h>
#include <chip/gpio.h>
#include <stm32f1xx_hal.h>

typedef struct stm_pin {
    ze_u16_t pin;
    ze_u32_t portbase;
    ze_u32_t mode;
    ze_u8_t level;
} stm_pin_t;

struct stm_pin stm_pins[]={
    {GPIO_PIN_5, GPIOB_BASE, GPIO_MODE_OUTPUT_PP, GPIO_LEVEL_HIGH},  //0
    {GPIO_PIN_5,GPIOE_BASE, GPIO_MODE_OUTPUT_PP, GPIO_LEVEL_HIGH},   //1
    {GPIO_PIN_12,GPIOB_BASE, GPIO_MODE_OUTPUT_PP, GPIO_LEVEL_HIGH},  //2
    {GPIO_PIN_2, GPIOD_BASE, GPIO_MODE_OUTPUT_PP, GPIO_LEVEL_HIGH},  //3
    {GPIO_PIN_7, GPIOD_BASE, GPIO_MODE_OUTPUT_PP, GPIO_LEVEL_HIGH},  //4
    {GPIO_PIN_10, GPIOF_BASE, GPIO_MODE_INPUT, GPIO_LEVEL_HIGH},     //5
    //{GPIO_PIN_2, GPIOB_BASE, GPIO_MODE_OUTPUT_PP, GPIO_LEVEL_HIGH},  //6
};

static void stm_set_port_clk(ze_u32_t portbase)
{
    switch(portbase)
    {
    case GPIOA_BASE:
      __HAL_RCC_GPIOA_CLK_ENABLE();
      break;
    case GPIOB_BASE:
      __HAL_RCC_GPIOB_CLK_ENABLE();
      break;
#if defined(GPIOC_BASE)
    case GPIOC_BASE:
      __HAL_RCC_GPIOC_CLK_ENABLE();
      break;
#endif
#if defined(GPIOD_BASE)
    case GPIOD_BASE:
      __HAL_RCC_GPIOD_CLK_ENABLE();
      break;
#endif
#if defined(GPIOE_BASE)
    case GPIOE_BASE:
      __HAL_RCC_GPIOE_CLK_ENABLE();
      break;
#endif
#if defined(GPIOF_BASE) 
    case GPIOF_BASE:
      __HAL_RCC_GPIOF_CLK_ENABLE();
      break;
#endif
#if defined(GPIOG_BASE)
    case GPIOG_BASE:
      __HAL_RCC_GPIOG_CLK_ENABLE();
      break;
#endif
#if defined(GPIOH_BASE)
    case GPIOH_BASE:
      __HAL_RCC_GPIOH_CLK_ENABLE();
      break;
#endif
#if defined(GPIOI_BASE)
    case GPIOI_BASE:
      __HAL_RCC_GPIOI_CLK_ENABLE();
      break;
#endif   
#if defined(GPIOJ_BASE)
    case GPIOJ_BASE:
      __HAL_RCC_GPIOJ_CLK_ENABLE();
      break;
#endif
#if defined(GPIOK_BASE)
    case GPIOK_BASE:
      __HAL_RCC_GPIOK_CLK_ENABLE();
      break;
#endif
    default:
      print("can not find this port!");
      break;
    }
}

static void stm_pin_config(struct stm_pin *stm_pin, ze_u32_t mode)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    stm_set_port_clk(stm_pin->portbase);

    GPIO_InitStruct.Pin = stm_pin->pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
        
    HAL_GPIO_Init((GPIO_TypeDef *)stm_pin->portbase, &GPIO_InitStruct);
}


int stm_gpio_set_level(gpio_chip_t* gpio_chip, ze_base_t pin, int value)
{
    stm_pin_t* stm_pin = stm_pins + pin;

    if(value == GPIO_LEVEL_LOW)
    {
        HAL_GPIO_WritePin((GPIO_TypeDef *)stm_pin->portbase, stm_pin->pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin((GPIO_TypeDef *)stm_pin->portbase, stm_pin->pin, GPIO_PIN_SET);
    }

    return 0;
}

int stm_gpio_get_level(gpio_chip_t* gpio_chip, ze_base_t pin)
{
    stm_pin_t* stm_pin = stm_pins + pin;

    if(HAL_GPIO_ReadPin((GPIO_TypeDef*)stm_pin->portbase, stm_pin->pin) == GPIO_PIN_RESET)
        return GPIO_LEVEL_LOW;
    else
        return GPIO_LEVEL_HIGH;
}

int stm_gpio_set_dir(gpio_chip_t* gpio_chip, ze_base_t pin, int value)
{
    stm_pin_t* stm_pin = stm_pins + pin;

    if(value == GPIO_DIR_INPUT)
        stm_pin_config(stm_pin, GPIO_MODE_INPUT);
    else
        stm_pin_config(stm_pin, GPIO_MODE_OUTPUT_PP);

    return 0;
}

int stm_gpio_get_dir(gpio_chip_t* gpio_chip, ze_base_t pin)
{

    return 0;
}

static gpio_chip_t stm_gpio_chip = {
    .base = 0,
    .ngpio = 6,
    .set_level = stm_gpio_set_level,
    .get_level = stm_gpio_get_level,
    .set_dir = stm_gpio_set_dir,
    .get_dir = stm_gpio_get_dir,
};

void stm_gpio_hw_init(void)
{
    stm_pin_t* stm_pin = stm_pins;

    for (int i = 0; i < sizeof(stm_pins) / sizeof(stm_pins[0]); i++)
    {
        stm_pin_config(stm_pin, GPIO_MODE_OUTPUT_PP);
        HAL_GPIO_WritePin((GPIO_TypeDef *)stm_pin->portbase, stm_pin->pin, stm_pin->level);
    }
        
    gpiochip_add(&stm_gpio_chip);
}