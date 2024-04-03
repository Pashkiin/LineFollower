#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SENSOR1 PC0
#define SENSOR2 PC1
#define SENSOR3 PC2
#define SENSOR4 PC3

#define LED PD0
#define MOTOR_LEFT_FORWARD PD4
#define MOTOR_LEFT_BACKWARD PD5
#define LEFT_MOTOR_SPEED OCR1B
#define RIGHT_MOTOR_SPEED OCR1A
#define MOTOR_RIGHT_FORWARD PD6
#define MOTOR_RIGHT_BACKWARD PD7
// Checking sensors, 1 means that sensor didn't detect black
uint8_t readSensors() {
	uint8_t sensors_values = 0000;
	if (bit_is_clear(PINC, SENSOR1)) sensors_values |= 1<<3;
	if (bit_is_clear(PINC, SENSOR2)) sensors_values |= 1<<2;
	if (bit_is_clear(PINC, SENSOR3)) sensors_values |= 1<<1;
	if (bit_is_clear(PINC, SENSOR4)) sensors_values |= 1<<0;
	return sensors_values;
}

void setupPWMandMOTORS() {

//	Setting pins D4-7 as output and PD0 for LED
	DDRD |= (
				(1 << DDD0) |
				(1 << DDD4) |
				(1 << DDD5) |
				(1 << DDD6) |
				(1 << DDD7));

//	Setting pins B1-2 as output for PWM
	DDRB |= (
			(1 << DDB1) |
			(1 << DDB2) );

//	Motors always enable
	PORTD |= (1 << MOTOR_LEFT_FORWARD);
	PORTD |= (1 << MOTOR_RIGHT_FORWARD);

//	Setting PWM on Timer/Counter 1 in 8bit mode
	TCCR1A |= (1 << WGM10) |
			  (1 << COM1A1) |
			  (1 << COM1B1);
	TCCR1B |= (1 << CS10) | (1 << WGM12);

//	Enable interrupts
	sei();
}

void setupSensors() {
//	Setting pins C0-3 as inputs
	DDRC &= ~( (1 << DDC0) |
				(1 << DDC1) |
				(1 << DDC2) |
				(1 << DDC3) );
}

void controlMotors(uint8_t sensorsBits) {
	unsigned char pelnaPIZDA = 255;
	unsigned char sredniaPIZDA = 128;
	unsigned char malaPIZDA = 64;
	unsigned char stopPIZDA = 0;

//  No black - full speed
	if (sensorsBits == 1111) {
		RIGHT_MOTOR_SPEED = pelnaPIZDA;
		LEFT_MOTOR_SPEED = pelnaPIZDA;
	}
//	Black on right edge sensor - right motor 1/4 of max speed
	if (sensorsBits == 1110 || sensorsBits == 1100) {
		RIGHT_MOTOR_SPEED = malaPIZDA;
		LEFT_MOTOR_SPEED = pelnaPIZDA;
	}
//	Black on middle right sensor - right motor 1/2 of max speed
	if (sensorsBits == 1101) {
		RIGHT_MOTOR_SPEED = sredniaPIZDA;
		LEFT_MOTOR_SPEED = pelnaPIZDA;
	}
//	Black on left edge sensor - left motor 1/4 of max speed
	if (sensorsBits == 0111 || sensorsBits == 0011) {
		RIGHT_MOTOR_SPEED = pelnaPIZDA;
		LEFT_MOTOR_SPEED = malaPIZDA;
	}
//	Black in left sensor - left motor 1/2 of max speed
	if (sensorsBits == 1011) {
		RIGHT_MOTOR_SPEED = pelnaPIZDA;
		LEFT_MOTOR_SPEED = sredniaPIZDA;
	}
//	Black on all sensors - STOP
	if (sensorsBits == 0000) {
		RIGHT_MOTOR_SPEED = stopPIZDA;
		LEFT_MOTOR_SPEED = stopPIZDA;
		//sprawdzenie czy to nie skrzyżowanie
	}

}


int main(void)
{

	setupPWMandMOTORS();
	setupSensors();

	while (1)
	{
	   uint8_t sensors = readSensors();

//	   Turn on led if black wasn't detected
	   if (sensors != 0b1111) {
		   PORTD &= (0 << PD0);
	   } else PORTD |= (1 << PD0);

	   controlMotors(sensors);
	   _delay_ms(6);
	}
}
