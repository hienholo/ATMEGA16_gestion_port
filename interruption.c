#define F_CPU 3686400
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile char x = 0xFE;  //x va nous servir comme variable pour l'indice de la LED à allumer. Il est volatile car peut être chnagé par l'intéruption


//Interruption
ISR(INT0_vect){
	x=0xFE;
}
//Fin interruption

int main(void)
{
	DDRB = 0xFF; //Met les ports B en mode sortie
	DDRD = 0x00; //Met les ports D en mode entrée
	
	while (1){
		GICR |= (1<<INT0);
		DDRD&=~(1<<PD2);
		MCUCR |= (1<<ISC01);
		sei();
		for (int i=0; i<8; i++){
			PORTB=x;
			x=(x<<1)+1;
			if (x==0xFF){
				x=0xFE;
			}
			_delay_ms(250);
		}
	}

}