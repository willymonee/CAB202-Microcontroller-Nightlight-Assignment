# CAB202-Microcontroller-Nightlight-Assignment

**Task** Invent, design, implement a prototype of a microntroller-based product/application which performs a meaningful service and carry out a specific useful function, developed using TinkerCad Circuits, and coded in AVR C for an Arduino UNO microcontroller. The chosen application was a nightlight. 

**Functionality**
Serial I/O – UART is used for serial input and output. Serial output is used to display instructions or feedback user via the console such as ‘enter the amount of time’. Serial input allows the user to input the amount of time they wish the nightlight to be on, enclosed in quotation marks e.g. “10”, via the console. 
Digital I/O - Switch	After the user has entered the desired time, the user will need to press the button switch to turn the nightlight on. The user may turn off the nightlight manually by pressing the switch button again. 
Digital I/O – Debouncing	Debouncing is used to accurately recognise a button click whereby the switch is pressed then released; preventing the recognition of multiple button clicks caused by bouncing. 
Digital I/O – LED (lightbulb)	Primary light source of the application, hence proving the main functionality of a nightlight. 
Analog Output – PWM	PWM is used to gradually dim the nightlight over a period of time. This is done by repetitively lowering the compare value which will increase the amount of time the PWM output PIN is set to low (lower the duty cycle), making the light appear dimmer. 
Analog Input – ADC	ADC is used to detect the brightness level from an ambient light sensor and convert this analogue value into a digital value upon button press. This digital value is then used to determine the initial duty cycle/brightness of the lightbulb, where a high value results in a low brightness of the lightbulb and vice versa. 
LCD	The LCD is used to display the countdown timer for the nightlight as well as its current brightness. It also displays instructions e.g. ‘enter time’ or ‘press button’. 
Timers (other than debouncing or PWM)	The timer has been used to cause an interrupt every second which will update the countdown time and current brightness of nightlight. This will display on the LCD as well as serial output the updated countdown time.
Advanced Functionality 
(LED matrix)	The LED matrix is a screen display for the user to look at. In this scenario it is in the shape of a star which is targeted towards children. This is implemented by multiplexing using a timer overflow interrupt. Though only one column of LEDs is on at a time, as they are turned on and off in quick succession, to the human eye the entire matrix appears on. 

**Video Demo**
https://youtu.be/p9GtenfXYtM

**Tinkercad Link**
https://www.tinkercad.com/things/26QCszVqaqE-assignment/editel?sharecode=0ZfNb6_uLFNxZWgBwMt_V7fRu6tkVYWaAUGInJFZSjg

See report for schematic and wiring instructions. 
