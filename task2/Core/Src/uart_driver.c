/*
 * uart_echo, uart_driver.c
 * Purpose: the implementation of the the UART driver.
 *
 * @author Oleksandr Ushkarenko
 * @version 1.0 31/08/2021
 */

#include "stm32f3xx.h"
#include "uart_driver.h"

/*
 * The function initializes USART2 by enabling clock source for USART2
 * and GPIOA. Pins PA2 (TX) and PA3 (RX) configured to work in alternate modes.
 */
void uart_init(void)
{
	// Enable clock access to USART2
	RCC->APB1ENR |= (1U << 17);

	// Enable clock access to GPIOA (bit 17)
	RCC->AHBENR |= (1U << 17);

	// Enable pins for alternate functions, PA2, PA3
	GPIOA->MODER &= ~0x000000F0;
	GPIOA->MODER |= 0x000000A0; 	// Enable alternative function for PA2, PA3

	// Configure type of alternate function
	GPIOA->AFR[0] &= ~0x0000FF00;
	GPIOA->AFR[0] |= 0x00007700;
}

/*
 * The function opens USART2 that includes enabling TX and RX modes,
 * setting baud rate and enabling USART2.
 */
void uart_open(void)
{
	// Configure USART2
	USART2->BRR = 8000000 / 9600; 	// 9600 baud
	USART2->CR1 = 0x0000000C; 		// Enable TX, RX, 8-bit data
	USART2->CR2 = 0x00000000;
	USART2->CR3 = 0x00000000;
	USART2->CR1 |= 0x00000001; 		// Enable USART2
}

/*
 * This blocking function waits until read data register is empty by checking flag RXNE (bit 5)
 * in interrupt and status register (ISR) and returns received data if RXNE is 1.
 */
int uart_read(void)
{
	while(!(USART2->ISR & 0x0020)) {}
	return USART2->RDR;
}

/*
 * This blocking function waits until transmit data register is not empty by checking flag TXE (bit 7)
 * in interrupt and status register (ISR) and transmits new data if TXE is 0.
 */
void uart_write(int ch)
{
	while(!(USART2->ISR & 0x0080)){}
	USART2->TDR = (ch & 0xFF);
}

/*
 * The function closes USART2 that includes disabling TX and RX modes,
 * setting 0 in the baud rate register and disabling USART2.
 */
void uart_close(void)
{
	USART2->CR1 = 0; // Disable USART
	USART2->BRR = 0;
	USART2->CR1 = 0; // Disable TX, RX
	USART2->CR2 = 0;
	USART2->CR3 = 0;
}

/*
 * The function deinitializes USART2 by disabling clock source for USART2
 * and GPIOA. Pins PA2 (TX) and PA3 (RX) configured to its default state.
 */
void uart_deinit(void)
{
	// Reset configuration of alternate function
	GPIOA->AFR[0] &= ~0x0000FF00;

	// Disable pins for alternate functions, PA2, PA3
	GPIOE->MODER &= ~0x000000F0;

	// Disable clock access to GPIOA (bit 17)
	RCC->AHBENR &= ~(1U << 17);

	// Disable clock access to USART2
	RCC->APB1ENR &= ~(1U << 17);
}
