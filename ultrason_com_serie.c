#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 3686400
#define BAUD 9600	//debit

#include <util/delay.h>
#include <stdint.h>
#define PIN_ECHO PIND6
static volatile unsigned int pulse = 0;

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

int main(void)
{
	usart_init(9600);
	usart_putc('t');
	char buffer[20];
	DDRB=0xFF; //Met les ports des LED en mode sortie
	PORTB=0xFF; //Eteint les LED
	
	DDRD  |= (1<<PD6); //PD6 (echo) en mode sortie
	DDRD &= ~(1<<PD2); //Met PD2 (trigger) en mode entrée
	_delay_ms(50);
	
	GICR|=(1<<INT0);  // External Interrupt Request & set (one) and the I-bit in the Status Register
	
	MCUCR|=(1<<ISC00)|(1<<ISC01); //define whether the External Interrupt is activated on rising
	//and/or falling edge of the INT0 pin or level sensed
	
	TCCR1A = 0;
	
	sei(); //Activateur global d'itteruption
	
	while(1)
	{
		PORTD|=(1<<PIN_ECHO);   //Envoi echo
		_delay_us(15);
		PORTD &=~(1<<PIN_ECHO); //Arret envoi echo
		_delay_us(150);
		
		//Affichage
		int distance = pulse/220;
		sprintf(buffer,"%d \n",distance);
		usart_puts(buffer); 
		
		_delay_ms(800);
		
	}
}
ISR(INT0_vect) //S'active si signal reçu sur la broche PD2.
{
	TCCR1B|=(1<<CS10);
	while(PIND&0x04);
	TCCR1B=0; //No clock source
	pulse=TCNT1;
	TCNT1=0;
	
}