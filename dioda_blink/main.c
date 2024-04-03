#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SENSOR1 PC0
#define SENSOR2 PC1
#define SENSOR3 PC2
#define SENSOR4 PC3

#define LED PD0
#define LEFT_MOTOR_FORWARD PD4
#define LEFT_MOTOR_BACKWARD PD5
#define LEFT_MOTOR_SPEED OCR1B
#define RIGHT_MOTOR_SPEED OCR1A
#define RIGHT_MOTOR_FORWARD PD6
#define RIGHT_MOTOR_BACKWARD PD7
// Checking sensors, 1 means that sensor didn't detect black
uint8_t readSensors() {
	uint8_t sensors_values = 0b0000;
	if (bit_is_clear(PINC, SENSOR1)) sensors_values |= 1<<3;
	if (bit_is_clear(PINC, SENSOR2)) sensors_values |= 1<<2;
	if (bit_is_clear(PINC, SENSOR3)) sensors_values |= 1<<1;
	if (bit_is_clear(PINC, SENSOR4)) sensors_values |= 1<<0;
	return sensors_values;
}

void setupPWMandMOTORS() {

//	Setting pins D4-7 as output and PD0 for LED
	DDRD |= (
				(1 << LED) |
				(1 << LEFT_MOTOR_FORWARD) |
				(1 << LEFT_MOTOR_BACKWARD) |
				(1 << RIGHT_MOTOR_FORWARD) |
				(1 << RIGHT_MOTOR_BACKWARD));

//	Setting pins B1-2 as output for PWM
	DDRB |= (
			(1 << PB1) |
			(1 << PB2) );

//	Motors always enable
	PORTD |= (
				(1 << LEFT_MOTOR_FORWARD) |
				(1 << RIGHT_MOTOR_FORWARD));

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
	DDRC &= ~(  (1 << SENSOR1) |
				(1 << SENSOR2) |
				(1 << SENSOR3) |
				(1 << SENSOR4) );
}

void controlMotors(uint8_t sensorsBits) {
	unsigned char full_speed = 255;
	unsigned char half_speed = 128;
	unsigned char low_speed = 64;
	unsigned char stop = 0;

//  No black - full speed
	if (sensorsBits == 0b1111) {
		RIGHT_MOTOR_SPEED = full_speed;
		LEFT_MOTOR_SPEED = full_speed;
	}
//	Black on right edge sensor - right motor 1/4 of max speed
	if (sensorsBits == 0b1110 || sensorsBits == 0b1100) {
		RIGHT_MOTOR_SPEED = low_speed;
		LEFT_MOTOR_SPEED = full_speed;
	}
//	Black on middle right sensor - right motor 1/2 of max speed
	if (sensorsBits == 0b1101) {
		RIGHT_MOTOR_SPEED = half_speed;
		LEFT_MOTOR_SPEED = full_speed;
	}
//	Black on left edge sensor - left motor 1/4 of max speed
	if (sensorsBits == 0b0111 || sensorsBits == 0b0011) {
		RIGHT_MOTOR_SPEED = full_speed;
		LEFT_MOTOR_SPEED = low_speed;
	}
//	Black in left sensor - left motor 1/2 of max speed
	if (sensorsBits == 0b1011) {
		RIGHT_MOTOR_SPEED = full_speed;
		LEFT_MOTOR_SPEED = half_speed;
	}
//	Black on all sensors - STOP
	if (sensorsBits == 0b0000) {
		RIGHT_MOTOR_SPEED = stop;
		LEFT_MOTOR_SPEED = stop;
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
	   	PORTD &= ~(1 << PD0);
	   } else PORTD |= (1 << PD0);

	   controlMotors(sensors);
	   _delay_ms(6);
	}
}
