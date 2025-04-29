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
 //Pour le joystick
 #define JOYSTICK_CHANNEL_Y 0 // Canal ADC pour l'entrée analogique du joystick
 #define JOYSTICK_CHANNEL_X 1  // Canal ADC pour l'entrée analogique du joystick
 //Pour l'ultrason
 #define PIN_ECHO PIND6
 static volatile unsigned int pulse = 0;
 
 
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
 
 // ____________ JOYSTICK __________
 uint16_t ADC_read(uint8_t channel) {
     ADMUX = (1 << REFS0) | (channel & 0x0F); 	// Configuration du registre ADMUX pour sélectionner le canal d'entrée ADC
     ADCSRA |= (1 << ADSC); 	// Demarrer la conversion ADC
     while (ADCSRA & (1 << ADSC)); 	// Attendre la fin de la conversion
     return ADC; // Retourner la valeur convertie
 }
 
 
 
 
 int main(void)
 {
     lcd_init();
     usart_init(9600);
     bt_init();
     char received_string[50];
     char buffer[50];
     
     //Joystick
     DDRA &= ~(1<<0) & ~(1<<1) & ~(1<<2); //Met les ports A 0, 1 et 2 en mode entrée
     PORTA |= (1 << 2); //Active la résitance de pull Up - utile pour utlisation du bouton SEL
     ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Activation de l'ADC et division par 64 de l'horloge
     
     //Divers
     DDRB = 0xFF; //Met les ports B en mode sortie - utile pour les LED
     command(0x01); //clean scren
 
     
     //CAPTEUR ULRASON
     DDRD  |= (1<<PD6); //PD6 (echo) en mode sortie
     DDRD &= ~(1<<PD2); //Met PD2 (trigger) en mode entrée
     _delay_ms(50);
     GICR|=(1<<INT0);  // External Interrupt Request & set (one) and the I-bit in the Status Register
     MCUCR|=(1<<ISC00)|(1<<ISC01); //define whether the External Interrupt is activated on rising	
     TCCR1A = 0;
     sei();
     
     while (1) 
     {
         //_____________________ JOYSTICK ________________________
         //Gestion de l'axe vertical du joystick
         uint16_t joystick_valueY = ADC_read(JOYSTICK_CHANNEL_Y);
         //Gestion de l'axe horizontal du joystick
         uint16_t joystick_valueX = ADC_read(JOYSTICK_CHANNEL_X);
         //Affichage
         sprintf(buffer,"X : %d Y : %d",joystick_valueX, joystick_valueY); //Affichage
         command(0x80);//Changement de ligne
         command(0x01); //clean screen
         print(buffer); //Affichage LCD
         sprintf(buffer,"X : %d Y : %d\n",joystick_valueX, joystick_valueY); //Affichage
         usart_puts(buffer); //Affichage BT
         
         //_____________________ DISTANCE ________________________
         PORTD|=(1<<PIN_ECHO);   //Envoi echo
         _delay_us(15);
         PORTD &=~(1<<PIN_ECHO); //Arret envoi echo
         _delay_us(150);
         
         int distance = pulse/220; //La valeur de 220 a été trouvé par expérimentation
         sprintf(buffer,"Distance : %d",distance); //Affichage
         command(0xC0);//Changement de ligne
         usart_puts(buffer); //Affichage LCD
         print(buffer); //Affichage LCD
         sprintf(buffer,"Distance : %d\n",distance); //Affichage
         usart_puts(buffer); //Affichage BT
         
         _delay_ms(1000); //Delai facultatif, mais plus simple pur l'affichage
 
         
     
         //____________ AUTRE ________________
         //Gestion du boutton du joystick
         if (PINA &= (1<<2)){
             PORTB = 0; //Allume les LED
             }else{
             PORTB = 0xFF; //Eteint les LED
         }
     }
 }
 
 
 // Interruption pour capteur ultrason
 ISR(INT0_vect) //S'active si signal reçu sur la broche PD2.
 {
     TCCR1B|=(1<<CS10);
     while(PIND&0x04);
     TCCR1B=0; //No clock source
     pulse=TCNT1;
     TCNT1=0;
     
 }