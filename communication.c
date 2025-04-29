/*
 * TP_SEM.c
 *
 * Created: 20/02/2024 08:22:49
 * Author : HIE HOLO
 */ 


 #define F_CPU 3686400
 #define BAUD 9600	//debit
 
 #include <avr/io.h>
 #include <util/delay.h>
 #include <avr/interrupt.h>
 
 void usart_init(unsigned int ubrr) {
     
     /* Enable receiver and transmitter */
     UCSRB |= (1 << TXEN) | (1 << RXEN) ;
     UCSRB &= ~(1 << UCSZ2);
     
     /* Set frame format: 8data, 1stop bit */
     UCSRC |= (1 << UCSZ1) | (3 << UCSZ0) ;
     unsigned long rate = F_CPU/ubrr;
     rate = rate/16UL - 1UL;
     UBRRL = rate;
     UBRRH = (rate >> 8);
 }
 
 void usart_putc(char c){
     while (!(UCSRA & (1 << UDRE)));
     UDR = c;
 }
 
 void usart_puts(char* s){
     while(*s){
         usart_putc(*s);
         s++;
     }
 }
 
 char usart_getc(void) {
     while (!(UCSRA & (1 << RXC)));
     return UDR;
 }
 
 int main(void) {
     char receivedChar;
 
     usart_init(9600); // Initialisation de l'USART
     while (1) {
         receivedChar = usart_getc(); // Réception d'un caractère
         usart_putc(receivedChar);   // Émission du caractère (écho)
         }
 
     return 0;
 }