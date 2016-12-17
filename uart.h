#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>

/* SETUP UART */
void USART_Init( unsigned int ubrr);

void USART_Interrupt_On(void);

/* Simple methods to make UART read and transmit more readble - Extremely unnecessary*/
void USART_Transmit( unsigned char data, FILE *stream);

unsigned char USART_Receive( FILE *stream );

void SendString(char *StringPtr);

#endif // UART_H 

