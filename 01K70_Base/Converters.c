
#include "Converters.h"

void ConvertIntToText(int dd, unsigned char *str)
{
	str[0]=dd/10000+48;
	str[1]=(dd/1000)-((dd/10000)*10)+48;
	str[2]=(dd/100)-((dd/1000)*10)+48;
	str[3]=(dd/10)-((dd/100)*10)+48;
	str[4]=dd-((dd/10)*10)+48;
	str[5]=0;
}

void ConvertUInt16ToText(uint16_t dd, unsigned char *str)
{
	uint8_t digits[5];
	uint16_t div = 10000;
	for(uint8_t i = 0 ; i < 4 ; i++)
	{
		digits[i] = dd / div;
		dd -= (digits[i] * div);
		div /= 10;
	}
	digits[4] = dd;
	for (uint8_t i = 0; i < 5; i++)
		str[i] = digits[i] + 48;
	str[5] = 0;
}

/* EOF */
