#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "HD44780.hpp"
#include "uart_buffer.hpp"


void ADC_TEMP_Init(void){
	// Uruchomienie ADC, wewnetrzne napiecie odniesienia, tryb pojedynczej konwersji, preskaler 128, wejscie PIN5, wynik do prawej
	ADCSRA = (1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
	//ADEN:7 - ADC Enable (uruchomienie przetwornika)
	//ADPS2:0 - prescaler (tu 128)

	// ADMUX  ADC Multiplexer Selection Register
	ADMUX |= (1<<REFS0) | 1<<MUX0; ADMUX &= ~(1<<MUX1) | ~(1<<MUX2) | ~(1<<MUX3);		//  MUX - input Channel Selections (0001 - ADC1)
	//REFS0:6 - Reference Selection Bits ->  Internal 1.1V Voltage Reference with external capacitor at AREF pin
}


int  ADC_conversion(){
	ADCSRA |= (1<<ADSC);		//	ADC - Start Conversion
	while(ADCSRA & (1<<ADSC)); 	//	wait for finish of conversion
	return ADC;
}


int main(){
	LCD_Initalize();
	ADC_TEMP_Init();
	uart_init(9600, 0);
	sei();

	char str[24];
	int measurement, fast_mode, continous_coversion = 0;
  uint8_t data;
	while(1){

		if (uart_read_count() > 0) {

			char command = uart_read();
      		data = uart_read();
			
			if (data == 'm')
      		{
       			 measurement = 1;
     	 	}
		
			if (data == 'f')
     		 {
       			 fast_mode = 1;
      		}
			if (data == 's')
      		{
				continous_coversion = 1;
     		 }

			if (data == 'b')
      		{
       			 fast_mode = 0;
				continous_coversion = 0;
				measurement = 0;
      		}
			_delay_ms(100);
		}

		if (measurement==1){
			int i = ADC;		
			i = ADC_conversion();
			_delay_ms(10);

			float temp = 5.0*i*100.0/1024;
			sprintf(str, "%d.%02d degC (%d)\r\n", (int)temp, (int)(temp*100)%100, i);

			LCD_GoTo(1,1);
			LCD_WriteText(str);
			uart_send_string((uint8_t*)str);
     
			measurement = 0;
		}
		if (continous_coversion==1){
			
			while (2)
			{
			int i = ADC;		
			i = ADC_conversion();
			_delay_ms(10);

			float temp = 5.0*i*100.0/1024;
			sprintf(str, "%d.%02d degC (%d)\r\n", (int)temp, (int)(temp*100)%100, i);

			LCD_GoTo(1,1);
			LCD_WriteText(str);
			uart_send_string((uint8_t*)str);

			_delay_ms(100);
			}
		
		}

		if (fast_mode==1){
			while (3)
			{
			int i = ADC;		
			i = ADC_conversion();
			_delay_ms(10);

			float temp = 5.0*i*100.0/1024;
			sprintf(str, "%d.%02d degC (%d)\r\n", (int)temp, (int)(temp*100)%100, i);

			LCD_GoTo(1,1);
			LCD_WriteText(str);
			uart_send_string((uint8_t*)str);
			
			}
		
		}

	}
	return 0;
}
