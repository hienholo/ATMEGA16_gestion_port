#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 3686400;

int main(void)
{
	char x = 0xFE;  //x va nous servir comme variable pour l'indice de la LED à allumer
	DDRB = 0xFF; //Met les ports B en mode sortie
	while (1)
	{
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