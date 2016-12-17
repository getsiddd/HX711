#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"

/* SETUP UART */
void USART_Init( unsigned int ubrr)
{
   /*Set baud rate */
   UBRR0H = (unsigned char)(ubrr>>8);
   UBRR0L = (unsigned char)ubrr;
   
  /*Enable receiver and transmitter */
   UCSR0B = (1<<RXEN0)|(1<<TXEN0);
   
   /* Set frame format: 8data, 2stop bit */
   UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}




/* Simple methods to make UART read and transmit more readble - Extremely unnecessary*/
void USART_Transmit( unsigned char data , FILE *stream)
{
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}


unsigned char USART_Receive( FILE *stream )
{
   return UDR0;
}

/*
void SendString(char *StringPtr)
{
    
    while(*StringPtr != 0x00)
    {  
    USART_Transmit(*StringPtr);    
    StringPtr++;
    }        

}
*/