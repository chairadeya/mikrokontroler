#ifndef F_CPU
	#define F_CPU 16000000UL
#endif


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "LCD.h"

#define RETRY_USER_ADDR 0x00
#define MEMORY_IS_EMPTY 1 
#define MEMORY_IS_FILLED 0
#define TURN_ON 1
#define TURN_OFF 0
#define INPUT_CORRECT 0
#define INPUT_WRONG 1


static unsigned char GetKeyPressed(){
	unsigned char x,c;

	PORTA |= 0xf0; // Pull Up for read data

	for(x=0;x<3;x++){
		DDRA &= ~(0xfe); // All pin set as input
		DDRA |= (0x08>>x); // Select Column

		for(c=0;c<4;c++){
			if(!(PINA & (0x80>>c))){
				return (c*3+x); // Get assorted number button array
			}
		}
	}
	
	return 0xff;
}


static uint8_t check_memory_data(void);
static void pb_buzzer(uint8_t state);
static void init_gpio(void);

int main(){
	init_gpio();

  	if (check_memory_data() == MEMORY_IS_EMPTY && (PINB & (1<<1)) == 1)
    	eeprom_update_byte(RETRY_USER_ADDR, 3);

  	uint8_t total_retry = eeprom_read_byte(RETRY_USER_ADDR);
  	uint8_t is_false = 0;
	char ch_retry[3];

	LCD4_init();
	LCD4_clear();

	unsigned char data, pos = 6;
	const unsigned char ch[12] = {
		'*', '0', '#',
		'7', '8', '9',
		'4', '5', '6',
		'1', '2', '3'
	};
	const char* pwd = "3241";
	char pass[5];
	const char* status = "Password";

	itoa(total_retry, ch_retry, 10); // 10 nya itu konversi ke bilangan desimal

	LCD4_move(0,0);
	LCD4_writes(status);

	LCD4_move(13,0);
	LCD4_write((unsigned char)'[');
	LCD4_move(14,0);
	LCD4_writes((char*)ch_retry);
	LCD4_move(15,0);
	LCD4_write((unsigned char)']');

	LCD4_move(5,1);
	LCD4_writes("[____]");
	
	if(total_retry < 1)
		return -1;

	while(1){
		data = GetKeyPressed();
		LCD4_move(pos,1);
		
		if(data != 0xff && data != 0 && data != 2){
			LCD4_write('*');
			
			if((pos-6) > 3){
				LCD4_clear();
				memset(pass,0,strlen(pass));
				status = "overflow";
			}
			pass[pos-6] = ch[data];
			pos++;
			_delay_ms(150);
		}

		LCD4_move((16-strlen(status))/2,0);
		LCD4_writes(status);
		if(data == 0){
			status = "true";
			for(int x=0;x<4;x++){
				if(pass[x] != pwd[x]){
          		is_false = 1;
				break;
				}
			}

      		if(is_false){
        		pb_buzzer(INPUT_WRONG);
        		if(total_retry > 0)
					status = "false";
        		else
            		status = "retry habis";  

        		total_retry--;
        		eeprom_update_byte(RETRY_USER_ADDR, total_retry);
      		}
      		else
        		pb_buzzer(INPUT_CORRECT);
				
		LCD4_clear();
		}
	}
}
	static uint8_t check_memory_data(void)
	{
    	uint8_t mem_value = eeprom_read_byte(RETRY_USER_ADDR);
   		 uint8_t is_empty = 0; //apakah kosong?

    	for(uint8_t check = 1; check<= 3; check++){
     		if(mem_value == check)
        		is_empty = 0;
      		else
        		is_empty = 1;
    	}
    	return is_empty;
	}

static void pb_buzzer(uint8_t state){
  	if(state){
    	for (uint8_t x=0; x< 5; x++){
      	PORTB ^= (1<<3); //PORTB DI XOR SAMA PORTB BIT 3 
      	_delay_ms(100);
   		 }
		PORTB &= ~(1<<3);
  	}
		
  	else
	{
   		PORTB |= (1<<3); 
    	_delay_ms(250);
   		PORTB &= ~(1<<3);
  	}
}

static void init_gpio(void){
  DDRB |= (1<<3);
  DDRD |= (1<<7);
  DDRB &= ~(1<<1);

  PORTB &= ~(1<<3);
  PORTD &= ~(1<<7); // register port itu buat output, pin buat input
}

