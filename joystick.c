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
 
 
 #define JOYSTICK_CHANNEL_Y 0 // Canal ADC pour l'entrée analogique du joystick
 #define JOYSTICK_CHANNEL_X 1  // Canal ADC pour l'entrée analogique du joystick
 
 ////////////////////////////BLOC Pour affichage serie //////////////////////////////////////////////
 void usart_init(unsigned int ubrr) {
     /* Enable transmitter */
     UCSRB |= (1 << TXEN) ;
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
 //////////////////////////////////////////////////////////////////////////
 
 
 uint16_t ADC_read(uint8_t channel) {
     ADMUX = (1 << REFS0) | (channel & 0x0F); 	// Configuration du registre ADMUX pour sélectionner le canal d'entrée ADC
     ADCSRA |= (1 << ADSC); 	// Demarrer la conversion ADC
     while (ADCSRA & (1 << ADSC)); 	// Attendre la fin de la conversion
     return ADC; // Retourner la valeur convertie
 }
 
 int main(void)
 {
     usart_init(9600); //initialise la liaison serie
     usart_putc('t'); //Permet de tester la liaison
     char buffer[20]; //tableau pour stocker le texte
     
     DDRA &= ~(1<<0) & ~(1<<1) & ~(1<<2); //Met les ports A 0, 1 et 2 en mode entrée		
     PORTA |= (1 << 2); //Active la résitance de pull Up - utile pour utlisation du bouton SEL
     DDRB = 0xFF; //Met les ports B en mode sortie - utile pour les LED
     ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Activation de l'ADC et division par 64 de l'horloge
 
     while (1) 
     {
         //Gestion de l'axe vertical du joystick
         uint16_t joystick_value = ADC_read(JOYSTICK_CHANNEL_Y);
         sprintf(buffer,"Vertical : %d \n",joystick_value); //Affichage
         usart_puts(buffer); //Affichage
         _delay_ms(800); //Delai facultatif, mais plus simple pur l'affichage
         
         //Gestion de l'axe horyzontal du joystick
         uint16_t joystick_value2 = ADC_read(JOYSTICK_CHANNEL_X);
         sprintf(buffer,"Horizontal : %d \n",joystick_value2); //Affichage
         usart_puts(buffer); //Affichage
         _delay_ms(800); //Delai facultatif, mais plus simple pur l'affichage
         
         //Gestion du boutton du joystick
         if (PINA &= (1<<2)){
             PORTB = 0; //Allume les LED
         }else{
             PORTB = 0xFF; //Eteint les LED
         }
         
     }
 }
 
 