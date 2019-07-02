#ifndef __UART_H__
#define __UART_H__

void uart_write(uint8_t *wr_data, int wr_len);
void uart_hw_init(void);
#endif