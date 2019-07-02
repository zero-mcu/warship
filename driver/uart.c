#include <common/zero.h>
#include <bus/serial.h>
#include "uart.h"
#include <stm32f1xx_hal.h>
#include <stm32f1xx_ll_usart.h>

IMPORT_SERIAL(0);

#define USE_UART0

#define UART0_INSTANCE           USART1
#define UART0_CLK_ENABLE()     __HAL_RCC_USART1_CLK_ENABLE()
#define UART0_CLK_SOURCE()     //LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1)
#define UART0_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART0_TX_PIN               GPIO_PIN_9
#define UART0_TX_GPIO_PORT              GPIOA
#define UART0_RX_PIN               GPIO_PIN_10
#define UART0_RX_GPIO_PORT              GPIOA
#define UART0_AF_CFG                    //GPIO_AF7_USART1
#define UART0_IRQ_NUM              USART1_IRQn
#define UART0_INTERRUPT              USART1_IRQHandler


#define UART_ENABLE_IRQ(n)            NVIC_EnableIRQ((n))
#define UART_DISABLE_IRQ(n)           NVIC_DisableIRQ((n))

struct stm_uart{
    USART_TypeDef *instance;
    UART_HandleTypeDef uart_handle;
    IRQn_Type irq;
};


int uart_init(serial_t *serial)
{
    struct stm_uart *uart = serial->private_data;
    UART_HandleTypeDef *huart = &uart->uart_handle;
    huart->Instance = uart->instance;

    huart->Init.BaudRate = serial->config.baudrate;
    if(serial->config.databits == SERIAL_DATABITS_8)
    {
        huart->Init.WordLength = UART_WORDLENGTH_8B;
    }
    else if(serial->config.databits == SERIAL_DATABITS_9)
    {
        huart->Init.WordLength = UART_WORDLENGTH_9B;
    }

    if(serial->config.stopbits == SERIAL_STOPBITS_1)
    {
        huart->Init.StopBits = UART_STOPBITS_1;
    }
    else if(serial->config.stopbits == SERIAL_STOPBITS_2)
    {
        huart->Init.StopBits = UART_STOPBITS_2;
    }

    if(serial->config.parity == SERIAL_PARITY_NONE)
    {
        huart->Init.Parity = UART_PARITY_NONE;
    }
    else if(serial->config.parity == SERIAL_PARITY_ODD)
    {
        huart->Init.Parity = UART_PARITY_ODD;
    }
    else if(serial->config.parity == SERIAL_PARITY_EVEN)
    {
        huart->Init.Parity = UART_PARITY_EVEN;
    }
    
    huart->Init.Mode = UART_MODE_TX_RX;

    huart->Init.OverSampling = UART_OVERSAMPLING_16;

    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;

    if(HAL_UART_Init(huart) != HAL_OK)
    {
        print("Uart hal driver init failed.");
        return -1;
    }

    __HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);

    return 0;
}

int uart_putc(serial_t *serial, int ch)
{
    struct stm_uart *uart;

    uart = serial->private_data;

    while(!LL_USART_IsActiveFlag_TXE(uart->instance));
    LL_USART_TransmitData8(uart->instance, ch);
    
    return 0;
}

int uart_getc(serial_t* serial)
{
    struct stm_uart* uart = (struct stm_uart*)serial->private_data;
    int ch = -1;

    if(LL_USART_IsActiveFlag_RXNE(uart->instance))
    {
        ch = LL_USART_ReceiveData8(uart->instance);
    }

    return ch;
}

const serial_ops_t uart_ops={
    .init = uart_init,
    .put_char = uart_putc,
    .get_char = uart_getc,
};



#ifdef USE_UART0
struct stm_uart uart0 = {
    .instance = UART0_INSTANCE,
    .irq = UART0_IRQ_NUM,
};

uint8_t uart0_rxbuf[SERIAL_RB_BUFSZ];

serial_fifo_t uart0_rx_fifo={
    .buffer = uart0_rxbuf,
    .put_index = 0,
    .get_index = 0,
    .bufsz = SERIAL_RB_BUFSZ,
};

void UART0_INTERRUPT(void)
{
    serial_hw_isr(SERIAL(0), SERIAL_EVENT_IRQ_RX);
}

#endif


static void uart_rcc_configuration(void)
{
#ifdef USE_UART0
    UART0_CLK_ENABLE();
    UART0_CLK_SOURCE();
    UART0_GPIO_CLK_ENABLE();
#endif
}

static void uart_gpio_configuration(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
#ifdef USE_UART0
    /*##-2- Configure peripheral GPIO ##########################################*/
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = UART0_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(UART0_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = UART0_RX_PIN;
    GPIO_InitStruct.Mode    = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull    = GPIO_NOPULL;
    HAL_GPIO_Init(UART0_RX_GPIO_PORT, &GPIO_InitStruct);
#endif
}

static void uart_nvic_configuration(struct stm_uart *uart)
{
    NVIC_SetPriority(uart->irq, 0);
    NVIC_EnableIRQ(uart->irq);
}

void uart_hw_init(void)
{
    uart_rcc_configuration();
    uart_gpio_configuration();

#ifdef USE_UART0
    uart_nvic_configuration(&uart0);

    SERIAL(0)->flags = SERIAL_FLAG_IRQ_RX;
    SERIAL_SET_CONFIG_DEFAULT(SERIAL(0));
    SERIAL(0)->serial_rx = &uart0_rx_fifo;
    SERIAL(0)->private_data = &uart0;
    serial_init(SERIAL(0), &uart_ops);
    const char* data = "hello";
#endif
}