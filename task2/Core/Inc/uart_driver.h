/*
 * uart_echo, uart_driver.h
 * Purpose: the header file of the UART driver.
 * UART driver should contain following functions:
 * init();
 * open();
 * read();
 * write();
 * close();
 * deinit();
 *
 * @author Oleksandr Ushkarenko
 * @version 1.0 31/08/2021
 */

#ifndef _UART_DRIVER_H_
#define _UART_DRIVER_H_

/*
 * UART driver function prototypes.
 */
void uart_init(void);
void uart_open(void);
int uart_read(void);
void uart_write(int data);
void uart_close(void);
void uart_deinit(void);

#endif

