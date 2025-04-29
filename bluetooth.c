/*
 * GccApplication1.c
 *
 * Created: 26/03/2024 13:20:26
 * Author : HIEN Holo
 */ 

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #define F_CPU 3686400
 #include <util/delay.h>
 #include <stdio.h>
 #include "i2c.h"
 
 //Pour le bluetooth
 #define RxD 0
 #define TxD 1
 
 //_____________ BT ___________________
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
 
 void bt_init(){
     _delay_ms(2000);
     usart_puts("AT+DEFAULT");
     _delay_ms(2000);
     usart_puts("AT+NAMEHBAUBRY");
     _delay_ms(400);
     usart_puts("AT+PIN0000");
     _delay_ms(400);
     //usart_puts("AT+AUTH1");
     //_delay_ms(400);
 }
 
 
 
 //__________________ LCD ____________________________________
 void lcd_init(){
     char buffer[20]; //tableau pour stocker le texte
     _delay_ms(50);
     I2C_Init();
     PORTB = 0x00;
     _delay_ms(50);
     uint8_t reponseACK = I2C_Start(0x7c);
     if(reponseACK == 1){
         PORTB = 0xFF;
     }
     _delay_ms(50);
     command(0x3C);
     _delay_us(50);
     command(0x0E);
     _delay_us(50);
     command(0x01);
     _delay_ms(2);
     command(0x06);
     sprintf(buffer,"Reponse : %d \n",reponseACK); //Mise en forme de la réponse
     print(buffer);
 }
 
 //____ AUTRES __________
 // Fonction pour lire une chaîne de caractères via UART
 void uart_receive_string(char* buffer, int buffer_size) {
     int i = 0;
     char received_char;
     // Lecture de caractères jusqu'à ce qu'un retour à la ligne soit reçu ou jusqu'à la taille maximale du buffer
     do {
         received_char = usart_getc();
         buffer[i] = received_char;
         i++;
     } while (received_char != '\n' && i < buffer_size - 1);
     // Ajout du caractère de fin de chaîne
     buffer[i] = '\0';
 }
 
 
 int main(void)
 {
     lcd_init();
     usart_init(9600);
     bt_init();
     char received_string[50];
     char buffer[50];
     
     while (1) 
     {
         uart_receive_string(received_string, sizeof(received_string));
         command(0x01);
 
         sprintf(buffer,received_string); //Mise en forme de la réponse
         print(buffer);
     }
 }