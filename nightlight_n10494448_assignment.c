// headers
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

// macro definitions
#define SET_BIT(reg, pin)           (reg) |= (1 << (pin))
#define SET_BITS(reg, mask)			(reg) |= mask
#define CLEAR_BIT(reg, pin)         (reg) &= ~(1 << (pin))
#define WRITE_BIT(reg, pin, value)  (reg) = (((reg) & ~(1 << (pin))) | ((value) << (pin)))
#define BIT_VALUE(reg, pin)         (((reg) >> (pin)) & 1)
#define BIT_IS_SET(reg, pin)        (BIT_VALUE((reg),(pin))==1)
#define CLEAR_BITS(reg, mask)		(reg) &= ~mask 
#define TOGGLE_BIT(reg, pin)	reg^= (1 << pin)

// constant definitions
#define BAUD_RATE 57600
//#define F_CPU 16000000
#define UBRR (F_CPU / 16 / BAUD_RATE - 1)

// LCD definitions copied from WK11 topic on LCDs
#define LCD_USING_4PIN_MODE (1)

#define LCD_USING_4PIN_MODE (1)

// #define LCD_DATA0_DDR (DDRD)
// #define LCD_DATA1_DDR (DDRD)
// #define LCD_DATA2_DDR (DDRD)
// #define LCD_DATA3_DDR (DDRD)
#define LCD_DATA4_DDR (DDRD)
#define LCD_DATA5_DDR (DDRD)
#define LCD_DATA6_DDR (DDRD)
#define LCD_DATA7_DDR (DDRD)


// #define LCD_DATA0_PORT (PORTD)
// #define LCD_DATA1_PORT (PORTD)
// #define LCD_DATA2_PORT (PORTD)
// #define LCD_DATA3_PORT (PORTD)
#define LCD_DATA4_PORT (PORTD)
#define LCD_DATA5_PORT (PORTD)
#define LCD_DATA6_PORT (PORTD)
#define LCD_DATA7_PORT (PORTD)

// #define LCD_DATA0_PIN (0)
// #define LCD_DATA1_PIN (1)
// #define LCD_DATA2_PIN (2)
// #define LCD_DATA3_PIN (3)
#define LCD_DATA4_PIN (4)
#define LCD_DATA5_PIN (5)
#define LCD_DATA6_PIN (6)
#define LCD_DATA7_PIN (7)


#define LCD_RS_DDR (DDRB)
#define LCD_ENABLE_DDR (DDRB)

#define LCD_RS_PORT (PORTB)
#define LCD_ENABLE_PORT (PORTB)

#define LCD_RS_PIN (1)
#define LCD_ENABLE_PIN (0)


//DATASHEET: https://s3-us-west-1.amazonaws.com/123d-circuits-datasheets/uploads%2F1431564901240-mni4g6oo875bfbt9-6492779e35179defaf4482c7ac4f9915%2FLCD-WH1602B-TMI.pdf

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

void lcd_init(void);
void lcd_write_string(uint8_t x, uint8_t y, char string[]);
void lcd_write_char(uint8_t x, uint8_t y, char val);
void lcd_clear(void);
void lcd_home(void);

void lcd_createChar(uint8_t, uint8_t[]);
void lcd_setCursor(uint8_t, uint8_t); 

void lcd_noDisplay(void);
void lcd_display(void);
void lcd_noBlink(void);
void lcd_blink(void);
void lcd_noCursor(void);
void lcd_cursor(void);
void lcd_leftToRight(void);
void lcd_rightToLeft(void);
void lcd_autoscroll(void);
void lcd_noAutoscroll(void);
void scrollDisplayLeft(void);
void scrollDisplayRight(void);

size_t lcd_write(uint8_t);
void lcd_command(uint8_t);

void lcd_send(uint8_t, uint8_t);
void lcd_write4bits(uint8_t);
void lcd_write8bits(uint8_t);
void lcd_pulseEnable(void);

uint8_t _lcd_displayfunction;
uint8_t _lcd_displaycontrol;
uint8_t _lcd_displaymode;

// function declarations
void uart_setup();
void uart_put_byte(unsigned char data);
int uart_get_byte(unsigned char *data);
void uart_transmit_string(char str[]);
void uart_receive_string(char buffer[], int buff_len);
int string_to_int(char buffer[]);
void reverse(char * str, int len);
void int_to_string(int x, char str[]);
void menu(void);
void process(void);
void setup(void);
void setup_Timer1(void);
void setup_Timer0(void);
void setup_Timer2(void);
void dim_bulb(int time);
void setup_adc(void);
uint16_t read_adc(void);
void setup_lcd(void);
void clear(void);
uint8_t my_delay(int);
void bulb_on(void);
void setup_led_matrix(void);
void lcd_write_brightness(void);

// global variables
int time_int;
char time_string[6] = {'\0'}; 
char brightness_string[10] = {'\0'};
uint16_t brightness = 0;
int time_selected;
volatile int elapsed_time = 0;
volatile uint8_t bit_count = 0;
volatile uint8_t switch_state = 0;
uint8_t prevState = 0;
volatile uint8_t matrix_column_count = 1;
uint8_t status = 0;


//**** SETUP FUNCTIONS ****//

// setup
void setup(void) {
	// PIN for lightbulb to output
	SET_BIT(DDRB, 3);
	// PIN for switch button to input 
	CLEAR_BIT(DDRB, 5);
	uart_setup();
	setup_adc();
	setup_lcd();
	setup_led_matrix();
	setup_Timer1();
	setup_Timer0();
	setup_Timer2();
	// enable interrupts
	sei();
	
}

// setup Timer 0 (used for debouncing button)
void setup_Timer0(void){
	// set prescaler to 256
	CLEAR_BITS(TCCR0B, (1 << CS00 | 1 << CS01));
	SET_BITS(TCCR0B, CS02);
	// enable timer overflow interrupt for timer 0
	SET_BIT(TIMSK0, TOIE0);
}

// setup Timer 1 (used for interrupting every 1 second)
void setup_Timer1(void) {
	// set prescaler to 1024
	CLEAR_BITS(TCCR1B, ( 1 << CS11));
	SET_BITS(TCCR1B,  (1 << CS12 | 1 << CS10));
	// normal mode
	TCCR1A = 0;
	//OCR = time x f_cpu / prescaler value
	// compare register value set at (every 1 second)
	OCR1AH = (uint8_t)(15625 >> 8);
	OCR1AL = (uint8_t)(15625);
}


// setup Timer 2 (used for PWM)
void setup_Timer2(void) {
	uint8_t mask; 
	// set compare match output mode to clear OC2A on compare match
	mask = 1 << COM2A1;
	SET_BITS(TCCR2A, mask);
	// set prescaler of timer to 8
	mask = 1 << CS21;
	SET_BITS(TCCR2B, mask);
	// set WGM to fast PWM and Top value to 255
	mask =  1 << WGM20 | 1 << WGM21;
	SET_BITS(TCCR2A, mask);
}

// setup LCD
void setup_lcd(void) {
  // set up the LCD in 4-pin or 8-pin mode
  lcd_init();

}

// setup ADC
void setup_adc(void) {
	// reference selection bit 
	SET_BIT(ADMUX, REFS0);
	// set input channel to ADC0 
	CLEAR_BITS(ADMUX, (1 << MUX0 | 1 << MUX1 | 1 << MUX2 | 1 << MUX3));
	// enable ADC and ADC interrupts
	SET_BITS(ADCSRA, (1 << ADEN));
	// set prescaler to 8 
	SET_BITS(ADCSRA, (1 << ADPS1 | 1 << ADPS0));
}

// setup UART settings
void uart_setup(void) {
	// set baud rate to 57600
	UBRR0 = UBRR;
	// enable receiver and transmitter
	SET_BITS(UCSR0B, (1 << RXEN0 | 1 << TXEN0));
	// set character size to 9 bits
	UCSR0C = (3 << UCSZ00);
	SET_BITS(UCSR0C, ( 1 << UCSZ00 | 1 << UCSZ01 | 1 << UCSZ02));
	// no parity + 1 stop bit ~ don't actually need to clear these
	 CLEAR_BITS(UCSR0C, ( 1 << UPM01 | 1 << UPM00));
}

// setup led matrix (set all the row and column pins to output)
void setup_led_matrix(void) {
	SET_BITS(DDRC, (1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5));
	SET_BITS(DDRD, (1 << 2 | 1 << 3));
	SET_BITS(DDRB, (1 << 2 | 1 << 4));
}

//**** PROCESSES ****//

// main function
int main() {
	setup();
	_delay_ms(200);
	// program waits for user input via serial input
	menu();
	while (1) {	
		// checks if the switch state has changed
		if (switch_state != prevState) {
			prevState = switch_state;
			// checks if the button is detected as pressed
			if (switch_state == 1 ) {
					// small delay to prevent registering 2 instances of the switch being high 
					// and ending the process function early
					_delay_ms(300);
					lcd_clear();
					process();
			}
        }
	}
	return 0;
}

// menu serial I/O
void menu(void) {
	lcd_write_string(0, 0, "Enter a time");
	uart_transmit_string("Please enter the amount of time: ");
	uart_receive_string(time_string, 6);
	time_selected = string_to_int(time_string);
	_delay_ms(10);
	int_to_string(time_int, time_string );
	// if the string to int function returns true meaning the user has entered a number value
	if(time_selected) {
		// output the value the user sent 
		int_to_string(time_int, time_string);
		uart_transmit_string(time_string);
	}
	else {
		// indicate that not time was selected
		uart_transmit_string("No time selected: night light will remain on indefinitely.");
		time_selected = 0;
	}
	uart_put_byte('\n');
	lcd_clear();
	_delay_ms(5);
	// disable uart receive temporarily 
	CLEAR_BIT(UCSR0B, RXEN0);
	lcd_write_string(0,0, "Press button");
	uart_transmit_string("Press button to start");
	uart_put_byte('\n');
}

// processes that occur after user inputs via menu/serial console
void process(void) {
	_delay_ms(5);
	status = 1;
	if (time_selected) {
		_delay_ms(5);
		// activate countdown ISR and set the counter to almost the compare value to trigger interrupt almost immediately
		SET_BIT(TIMSK1, OCIE1A);
		TCNT1 = 13000;
		// run the dim bulb function to dim the light bulb over the period of time selected
		dim_bulb(time_int);
	}
	else {
		SET_BIT(TIMSK1, OCIE1A);
		TCNT1 = 13000;
		// run the bulb on function to turn on the light bulb *no dimming 
		bulb_on();
	}
}

// turn on light bulb and dim it over time
void dim_bulb(int time) {
	uint8_t count = 1;
	int ocr;
	read_adc();
	// determine the initial compare value (duty cycle) depending on the value read from the ADC
	// the greater the ADC value the lower the duty cycle (dimmer the bulb will be) and vice versa
	if (brightness > 700) {
		ocr = 68.0; 
		uart_transmit_string("Surrounding is bright. Brightness level of light set to low");
		uart_put_byte('\n');
	}
	else if (brightness < 250) {
		ocr = 255.0;
		uart_transmit_string("Surrounding is dark. Brightness level of light set to high");
		uart_put_byte('\n');
	}
	else {
		ocr = 190.0;
		uart_transmit_string("Surrounding is neither bright nor dark. Brightness level of light set to medium");
		uart_put_byte('\n');
	}
	// calculate the time required between each decrement of the compare value for x amount of time.
	// note that ordinarily this would be 1000 * time / ocr, however due to artificial delays this number has been adjusted
	int delay = 950 * time / ocr;	
	_delay_ms(5);
	// decrement the compare value reducing the duty cycle and hence brightness, every 'delay' amount of time
	do {
		// within the delay function it checks whether the button is pressed which will end the process
		if (my_delay(delay) == 1) {
			clear();
			break;
		}
		OCR2A = ocr - count;
		count++;
		// check outside the delay function whether the button is pressed which will end the process
		if (switch_state != prevState) {
			prevState = switch_state;
			if (switch_state == 1 ) {
				clear();
				break;
			}
		}
	}
	// until the compare value reaches 0 whereby the lightbulb will be off
	while (OCR2A != 0);
}

// Turn the light bulb on but do not dim it overtime
void bulb_on(void) {
	double ocr;
	lcd_write_string(0, 0, "No dimming");
	read_adc();
	// determine the compare value (duty cycle) depending on the value read from the ADC
	// the greater the ADC value the lower the duty cycle (dimmer the bulb will be) and vice versa
	if (brightness > 700) {
		ocr = 68.0; 
		uart_transmit_string("Surrounding is bright. Brightness level of light set to low");
		uart_put_byte('\n');
	}
	else if (brightness < 250) {
		ocr = 255.0;
		uart_transmit_string("Surrounding is dark. Brightness level of light set to high");
		uart_put_byte('\n');
	}
	else {
		ocr = 190.0;
		uart_transmit_string("Surrounding is neither bright nor dark. Brightness level of light set to medium");
		uart_put_byte('\n');
	}
	// set the compare value/duty cycle and keep it constant until a button press is detected which will stop this process
	OCR2A = ocr;
	while (switch_state != 1){};
	clear();
}


//**** Interrupts ****//

// Interrupt that triggers every second of the countdown 
ISR(TIMER1_COMPA_vect) {
	if (time_selected) {
		// calculate the time remaining and convert it into a string 
		char time_remaining_string[10] = {'\0'};
		int time_remaining = time_int - elapsed_time;
		int_to_string(time_remaining, time_remaining_string);
		elapsed_time++;
		// reset the counter 
		TCNT1 = 0;
		if (time_remaining == 0) {
			// turn off timer 1 compare interrupt 
			CLEAR_BIT(TIMSK1, OCIE1A);
			uart_transmit_string("0");
			lcd_write_string(6, 0, "0");
			lcd_write_string(0, 1, "Goodnight!");
			uart_put_byte('\n');
			// stop sending 5v through the columns of the LED matrix (turning it off)
			CLEAR_BITS(PORTC, (1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5));
			_delay_ms(1000);
			// clear global variables such as elapsed time & LCD
			clear();
		}
		else {
			// display the time remaining via serial output
			uart_transmit_string(time_remaining_string);
			uart_put_byte('\n');
			// display the time remaining via the LCD 
			lcd_clear();
			lcd_write_string(0, 0, "Time:");
			lcd_write_string(6, 0, time_remaining_string);
			// display the current led brightness
			lcd_write_brightness();
		}
	}
	else {
		// just display the current brightness 
		lcd_write_brightness();
	}
}

// Interrupt for debouncing and multiplexing the LED matrix 
ISR(TIMER0_OVF_vect) {
	// debouncing the button 
	uint8_t mask = 0b00111111;
	uint8_t value = BIT_VALUE(PINB, 5);
	bit_count = ((bit_count << 1) & mask) | value;
	// requires 6 or more consecutive overflow readings of 1 from the button PIN to indicate button is pressed
	if(bit_count == mask) {
		switch_state = 1;
	}
	// requires 6 or more consecutive overflow readings of 0 from the button PIN to indicate that the button is not pressed
	else if (bit_count == 0) {
		switch_state = 0;
	}
	
	// reset all the columns and rows so no LEDs in the matrix are on 
	CLEAR_BITS(PORTC, (1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5));
	SET_BITS(PORTD, (1 << 2 | 1 << 3));
	SET_BITS(PORTB, (1 << 2 | 1 << 4));
	
	// if the process function is running 
	// every overflow turn only one column on and its respective rows, turn other columns off 
	// cycle through columns after each overflow 
	if (status == 1) {
		if (matrix_column_count == 1) {
			// send voltage through the first column 
			SET_BIT(PORTC, 1);
			// ground the specific rows you want the LED to be on
			CLEAR_BIT(PORTB, 4);
			CLEAR_BIT(PORTD, 3);
			// set the specific rows you do not want to be on 
			SET_BIT(PORTD, 2);
			SET_BIT(PORTB, 2);
		}
		else if (matrix_column_count == 2) {
			SET_BIT(PORTC, 2);
			CLEAR_BIT(PORTB, 2);
			CLEAR_BIT(PORTD, 3);
			SET_BIT(PORTD, 2);
			SET_BIT(PORTB, 4);
		}
		else if (matrix_column_count == 3) {
			SET_BIT(PORTC, 3);
			CLEAR_BITS(PORTD, (1 << 2 | 1 << 3));
			CLEAR_BIT(PORTB, 2);
			SET_BIT(PORTB, 4);
		}
		else if (matrix_column_count == 4) {
			SET_BIT(PORTC, 4);
			CLEAR_BIT(PORTB, 2);
			CLEAR_BIT(PORTD, 3);
			SET_BIT(PORTD, 2);
			SET_BIT(PORTB, 4);
		}
		else {
			SET_BIT(PORTC, 5);
			CLEAR_BIT(PORTB, 4);
			CLEAR_BIT(PORTD, 3);
			SET_BIT(PORTD, 2);
			SET_BIT(PORTB, 2);
			// once reaching the final column count reset column count
			matrix_column_count = 0;
		}
		matrix_column_count++;
	}

}


//**** FUNCTIONS ****//

// print the current brightness level of the light bulb via LCD
void lcd_write_brightness(void){
	if (OCR2A > 190) {
			lcd_write_string(0, 1, "Light: Bright");
		}
		else if (OCR2A < 68) {
			lcd_write_string(0, 1, "Light: Dim");
		}
		else {
			lcd_write_string(0, 1, "Light: Medium");
		}
}

// take a variable int and produce a delay of that time e.g. if the provided argument was 10
// this function would produce 10 1ms delays
// function returns 1 if the button is clicked during the delay process and 0 if there were no clicks
uint8_t my_delay(int delay) {
	 while(delay--) {
		_delay_ms(1);
		if (switch_state != prevState) {
			prevState = switch_state;
			if (switch_state == 1 ) {
			
				return 1;
			}
		}
	 }
	 return 0;
}

// read ADC once button is pressed 
uint16_t read_adc() {
	// start ADC conversion
	SET_BIT(ADCSRA, ADSC);
	// wait for buffer to empty/conversion to complete
	while (ADCSRA & (1 << ADSC));
	brightness = ADC;
	int_to_string(brightness, brightness_string);
	//uart_transmit_string(brightness_string);
	return (ADC);
}

// once process is finished --> reset everything 
void clear(void) {
	OCR2A = 0;
	uart_transmit_string("Goodnight!");
	uart_put_byte('\n');
	CLEAR_BIT(TIMSK1, OCIE1A);
	lcd_clear();
	elapsed_time = 0;
	time_selected = 0;
	time_int = 0;
	brightness = 0;
	memset(time_string, 0, 6);
	status = 0;
	CLEAR_BITS(PORTC, (1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5));
	// re-enable the uart receive
	SET_BIT(UCSR0B, RXEN0);
	// after clearing return back to the menu (serial I/O)
	menu();
}

// UART functions adapted from WK8 AMS send and receive exercise

// send a string through serial output
void uart_transmit_string(char str[]) {
	int i = 0;
	while (str[i] != '\0') {
		uart_put_byte((unsigned char)(str[i]));
		i++;
	}
	uart_put_byte('\0');
}

// send one byte through serial output
void uart_put_byte(unsigned char data) {
	// wait for empty transmit buffer
	while (!BIT_IS_SET(UCSR0A, UDRE0));
	// place data in data register 
	UDR0 = data;
}

// receives one byte through serial input
int uart_get_byte(unsigned char *data) {
    // If receive buffer contains data...
    if (BIT_IS_SET(UCSR0A, RXC0)) {
        // Copy received byte from UDR0 into memory location (*buffer)
        *data = UDR0;
        return 1;
    }
    else {
        return 0;
    }
}

// receive a string through serial input
void uart_receive_string(char buffer[], int buff_len) {
	int i = 0; // number of characters that have been added to the char array
	unsigned char ch;  // tempory location to read bytes (
	// Copy received byte from UDR0 into memory location (*buffer)
	
	// return each byte and store it into a char array
	// only stores characters in between 
	while(1) {
		// if it returns , then there is no byte available
		// though you are also storing shit in ch
		while(!uart_get_byte(&ch)) {
			// do nothing
		}
		if (ch == '"' && i == 0) {
			// clear the buffer 
			memset(buffer, 0, buff_len);
		}
		// end of string once reading a double quotation mark
		else if (ch == '"') {
			break;
		}
		// check if there is enough space in the char array - 1 for an ending null character
		else if (i < (buff_len-1)) {
			buffer[i] = ch;
			i++;
		}
	}
	buffer[i] = '\0';
}

// convert char to int
int string_to_int(char buffer[]) { 
	sscanf(buffer, "%d", &time_int);
	if (time_int == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

// integer to string conversion functions below adapted from WK10 example 1 file
// Reverses a string 'str' of length 'len' 
void reverse(char * str, int len) {
  int i = 0, j = len - 1, temp;
  while (i < j) {
    temp = str[i];
    str[i] = str[j];
    str[j] = temp;
    i++;
    j--;
  }
}

// convert an integer to string 
void int_to_string(int x, char str[]) {
	int num_digits = log10(x) + 1;
	for (int i = 0; i < num_digits; i++, x /= 10) {
		str[i] = (x % 10) + '0';
	}
	reverse(str, num_digits);
	str[num_digits] = '\0';
}

/* ********************************************/
// START LIBRARY FUNCTIONS - copied from WK11 LCD topic 

void lcd_init(void){
  //dotsize
  if (LCD_USING_4PIN_MODE){
    _lcd_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  } else {
    _lcd_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
  }
  
  _lcd_displayfunction |= LCD_2LINE;

  // RS Pin
  LCD_RS_DDR |= (1 << LCD_RS_PIN);
  // Enable Pin
  LCD_ENABLE_DDR |= (1 << LCD_ENABLE_PIN);
  
  #if LCD_USING_4PIN_MODE
    //Set DDR for all the data pins
    LCD_DATA4_DDR |= (1 << LCD_DATA4_PIN);
    LCD_DATA5_DDR |= (1 << LCD_DATA5_PIN);
    LCD_DATA6_DDR |= (1 << LCD_DATA6_PIN);    
    LCD_DATA7_DDR |= (1 << LCD_DATA7_PIN);

  #else
    //Set DDR for all the data pins
    LCD_DATA0_DDR |= (1 << LCD_DATA0_PIN);
    LCD_DATA1_DDR |= (1 << LCD_DATA1_PIN);
    LCD_DATA2_DDR |= (1 << LCD_DATA2_PIN);
    LCD_DATA3_DDR |= (1 << LCD_DATA3_PIN);
    LCD_DATA4_DDR |= (1 << LCD_DATA4_PIN);
    LCD_DATA5_DDR |= (1 << LCD_DATA5_PIN);
    LCD_DATA6_DDR |= (1 << LCD_DATA6_PIN);
    LCD_DATA7_DDR |= (1 << LCD_DATA7_PIN);
  #endif 

  // SEE PAGE 45/46 OF Hitachi HD44780 DATASHEET FOR INITIALIZATION SPECIFICATION!

  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
  _delay_us(50000); 
  // Now we pull both RS and Enable low to begin commands (R/W is wired to ground)
  LCD_RS_PORT &= ~(1 << LCD_RS_PIN);
  LCD_ENABLE_PORT &= ~(1 << LCD_ENABLE_PIN);
  
  //put the LCD into 4 bit or 8 bit mode
  if (LCD_USING_4PIN_MODE) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    lcd_write4bits(0b0111);
    _delay_us(4500); // wait min 4.1ms

    // second try
    lcd_write4bits(0b0111);
    _delay_us(4500); // wait min 4.1ms
    
    // third go!
    lcd_write4bits(0b0111); 
    _delay_us(150);

    // finally, set to 4-bit interface
    lcd_write4bits(0b0010); 
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    lcd_command(LCD_FUNCTIONSET | _lcd_displayfunction);
    _delay_us(4500);  // wait more than 4.1ms

    // second try
    lcd_command(LCD_FUNCTIONSET | _lcd_displayfunction);
    _delay_us(150);

    // third go
    lcd_command(LCD_FUNCTIONSET | _lcd_displayfunction);
  }

  // finally, set # lines, font size, etc.
  lcd_command(LCD_FUNCTIONSET | _lcd_displayfunction);  

  // turn the display on with no cursor or blinking default
  _lcd_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  lcd_display();

  // clear it off
  lcd_clear();

  // Initialize to default text direction (for romance languages)
  _lcd_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  lcd_command(LCD_ENTRYMODESET | _lcd_displaymode);
}


/********** high level commands, for the user! */
void lcd_write_string(uint8_t x, uint8_t y, char string[]){
  lcd_setCursor(x,y);
  for(int i=0; string[i]!='\0'; ++i){
    lcd_write(string[i]);
  }
}

void lcd_write_char(uint8_t x, uint8_t y, char val){
  lcd_setCursor(x,y);
  lcd_write(val);
}

void lcd_clear(void){
  lcd_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  _delay_us(2000);  // this command takes a long time!
}

void lcd_home(void){
  lcd_command(LCD_RETURNHOME);  // set cursor position to zero
  _delay_us(2000);  // this command takes a long time!
}


// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  lcd_command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    lcd_write(charmap[i]);
  }
}


void lcd_setCursor(uint8_t col, uint8_t row){
  if ( row >= 2 ) {
    row = 1;
  }
  
  lcd_command(LCD_SETDDRAMADDR | (col + row*0x40));
}

// Turn the display on/off (quickly)
void lcd_noDisplay(void) {
  _lcd_displaycontrol &= ~LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}
void lcd_display(void) {
  _lcd_displaycontrol |= LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

// Turns the underline cursor on/off
void lcd_noCursor(void) {
  _lcd_displaycontrol &= ~LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}
void lcd_cursor(void) {
  _lcd_displaycontrol |= LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

// Turn on and off the blinking cursor
void lcd_noBlink(void) {
  _lcd_displaycontrol &= ~LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}
void lcd_blink(void) {
  _lcd_displaycontrol |= LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | _lcd_displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_leftToRight(void) {
  _lcd_displaymode |= LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | _lcd_displaymode);
}

// This is for text that flows Right to Left
void lcd_rightToLeft(void) {
  _lcd_displaymode &= ~LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | _lcd_displaymode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll(void) {
  _lcd_displaymode |= LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | _lcd_displaymode);
}

// This will 'left justify' text from the cursor
void lcd_noAutoscroll(void) {
  _lcd_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | _lcd_displaymode);
}

/*********** mid level commands, for sending data/cmds */

inline void lcd_command(uint8_t value) {
  //
  lcd_send(value, 0);
}

inline size_t lcd_write(uint8_t value) {
  lcd_send(value, 1);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void lcd_send(uint8_t value, uint8_t mode) {
  //RS Pin
  LCD_RS_PORT &= ~(1 << LCD_RS_PIN);
  LCD_RS_PORT |= (!!mode << LCD_RS_PIN);

  if (LCD_USING_4PIN_MODE) {
    lcd_write4bits(value>>4);
    lcd_write4bits(value);
  } else {
    lcd_write8bits(value); 
  } 
}

void lcd_pulseEnable(void) {
  //Enable Pin
  LCD_ENABLE_PORT &= ~(1 << LCD_ENABLE_PIN);
  _delay_us(1);    
  LCD_ENABLE_PORT |= (1 << LCD_ENABLE_PIN);
  _delay_us(1);    // enable pulse must be >450ns
  LCD_ENABLE_PORT &= ~(1 << LCD_ENABLE_PIN);
  _delay_us(100);   // commands need > 37us to settle
}

void lcd_write4bits(uint8_t value) {
  //Set each wire one at a time

  LCD_DATA4_PORT &= ~(1 << LCD_DATA4_PIN);
  LCD_DATA4_PORT |= ((value & 1) << LCD_DATA4_PIN);
  value >>= 1;

  LCD_DATA5_PORT &= ~(1 << LCD_DATA5_PIN);
  LCD_DATA5_PORT |= ((value & 1) << LCD_DATA5_PIN);
  value >>= 1;

  LCD_DATA6_PORT &= ~(1 << LCD_DATA6_PIN);
  LCD_DATA6_PORT |= ((value & 1) << LCD_DATA6_PIN);
  value >>= 1;

  LCD_DATA7_PORT &= ~(1 << LCD_DATA7_PIN);
  LCD_DATA7_PORT |= ((value & 1) << LCD_DATA7_PIN);

  lcd_pulseEnable();
}

void lcd_write8bits(uint8_t value) {
  //Set each wire one at a time

  #if !LCD_USING_4PIN_MODE
    LCD_DATA0_PORT &= ~(1 << LCD_DATA0_PIN);
    LCD_DATA0_PORT |= ((value & 1) << LCD_DATA0_PIN);
    value >>= 1;

    LCD_DATA1_PORT &= ~(1 << LCD_DATA1_PIN);
    LCD_DATA1_PORT |= ((value & 1) << LCD_DATA1_PIN);
    value >>= 1;

    LCD_DATA2_PORT &= ~(1 << LCD_DATA2_PIN);
    LCD_DATA2_PORT |= ((value & 1) << LCD_DATA2_PIN);
    value >>= 1;

    LCD_DATA3_PORT &= ~(1 << LCD_DATA3_PIN);
    LCD_DATA3_PORT |= ((value & 1) << LCD_DATA3_PIN);
    value >>= 1;

    LCD_DATA4_PORT &= ~(1 << LCD_DATA4_PIN);
    LCD_DATA4_PORT |= ((value & 1) << LCD_DATA4_PIN);
    value >>= 1;

    LCD_DATA5_PORT &= ~(1 << LCD_DATA5_PIN);
    LCD_DATA5_PORT |= ((value & 1) << LCD_DATA5_PIN);
    value >>= 1;

    LCD_DATA6_PORT &= ~(1 << LCD_DATA6_PIN);
    LCD_DATA6_PORT |= ((value & 1) << LCD_DATA6_PIN);
    value >>= 1;

    LCD_DATA7_PORT &= ~(1 << LCD_DATA7_PIN);
    LCD_DATA7_PORT |= ((value & 1) << LCD_DATA7_PIN);
    
    lcd_pulseEnable();
  #endif
}

