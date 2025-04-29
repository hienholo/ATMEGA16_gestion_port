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
 
 
 
 #define JOYSTICK_CHANNEL_Y 0 // Canal ADC pour l'entrée analogique du joystick
 #define JOYSTICK_CHANNEL_X 1  // Canal ADC pour l'entrée analogique du joystick
 
 
 uint16_t ADC_read(uint8_t channel) {
     ADMUX = (1 << REFS0) | (channel & 0x0F); 	// Configuration du registre ADMUX pour sélectionner le canal d'entrée ADC
     ADCSRA |= (1 << ADSC); 	// Demarrer la conversion ADC
     while (ADCSRA & (1 << ADSC)); 	// Attendre la fin de la conversion
     return ADC; // Retourner la valeur convertie
 }
 
 int main(void)
 {
     ///////////////// AFFICHAGE LCD /////////
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
     /////////////////////////////////////////////
 
     
     DDRA &= ~(1<<0) & ~(1<<1) & ~(1<<2); //Met les ports A 0, 1 et 2 en mode entrée		
     PORTA |= (1 << 2); //Active la résitance de pull Up - utile pour utlisation du bouton SEL
     ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Activation de l'ADC et division par 64 de l'horloge
 
     while (1) 
     {
         //Gestion de l'axe vertical du joystick
         uint16_t joystick_value = ADC_read(JOYSTICK_CHANNEL_Y);
         sprintf(buffer,"Vertical : %d \n",joystick_value); //Affichage
         command(0x80);//Changement de ligne
         print(buffer); //Affichage
         _delay_ms(800); //Delai facultatif, mais plus simple pur l'affichage
         
         //Gestion de l'axe horizontal du joystick
         uint16_t joystick_value2 = ADC_read(JOYSTICK_CHANNEL_X);
         sprintf(buffer,"Horizontal : %d \n",joystick_value2); //Affichage
         command(0xC0);//Changement de ligne
         print(buffer);
         _delay_ms(800); //Delai facultatif, mais plus simple pur l'affichage
     }
 }