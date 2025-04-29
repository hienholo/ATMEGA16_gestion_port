#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 10000000L
#include <util/delay.h>
#include <stdint.h>
#define PIN_ECHO PIND6
static volatile unsigned int pulse = 0;

int main(void)
{
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
		
		int distance = pulse/220; //La valeur de 220 a été trouvé par expérimentation
		if((distance)<= 10){
			PORTB = 0; //Allume les lampes si distances < 10cm
			pulse=0;
		}
		else{
			PORTB = 0xFF;
			pulse=0;
		}
		_delay_ms(200);
		
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