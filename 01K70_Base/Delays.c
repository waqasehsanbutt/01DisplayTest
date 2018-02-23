#include "Delays.h"

void Delay_Short(void){
	Delay_Cycles(100);
}


void Delay_Single(unsigned int val){
	Delay_Cycles(val * 1000);
}

void Delay_Cycles(int val){
	for (; val > 0 ; val--);
}

// Delay for val milliseconds
void Delay_ms(unsigned int val){
	unsigned int i,j;
	for(i=0; i<val; i++)
		for(j=0; j< 20000; j++);
}  

/* EOF */
