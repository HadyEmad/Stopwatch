/*
 * Mini_Project2.c
 *
 *  Created on: 12 Sept 2021
 *      Author: Hady Emad
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int seconds = 0;
int minutes = 0;
int hours = 0;

ISR(TIMER1_COMPA_vect) {
	seconds++;
	if (seconds == 60) {
		minutes++;
		seconds = 0;
		if (minutes == 60) {
			hours++;
			minutes = 0;
		}
	}
}

void timer1_init(void) {
	SREG &= ~(1 << 7);
	TCNT1 = 0;
	OCR1A = 977;
	TIMSK = (1 << OCIE1A);
	TCCR1A = (1 << FOC1A);
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
	SREG |= (1 << 7);
}

// Configuring interrupt 0 to reset the stopwatch by resetting hours, minutes and seconds to 0
// and to be triggered on falling edge.

ISR(INT0_vect) {
	seconds = 0;
	minutes = 0;
	hours = 0;
}

void INT0_init(void) {
	SREG &= ~(1 << 7);
	GICR |= (1 << INT0);
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);
	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1 << ISC00);
	SREG |= (1 << 7);
}

// Configuring interrupt 1 to pause the stopwatch by stopping timer1
//and to be triggered on rising edge.

ISR (INT1_vect) {
	TCCR1B &= ~(1 << CS10) & ~(1 << CS12);
}

void INT1_init(void) {
	SREG &= ~(1 << 7);
	GICR |= (1 << INT1);
	DDRD &= ~(1 << PD3);
	MCUCR |= (1 << ISC11) | (1 << ISC10);
	SREG |= (1 << 7);
}

// Configuring interrupt 2 to resume the stopwatch by enabling timer1 again
// and to be triggered on falling edge.

ISR(INT2_vect) {
	TCCR1B |= (1 << CS10) | (1 << CS12);
}

void INT2_init(void) {
	SREG &= ~(1 << 7);
	GICR |= (1 << INT2);
	DDRB &= ~(1 << PB2);
	PORTB |= (1 << PB2);
	MCUCSR &= ~(1 << ISC2);
	SREG |= (1 << 7);
}

//  Function to display time on the 7 segments.

void display(void) {
	PORTA = (PORTA & 0XC0) | (1 << PA0);
	PORTC = (PORTC & 0XF0) | (seconds % 10);
	_delay_ms(2);
	PORTA = (PORTA & 0XC0) | (1 << PA1);
	PORTC = (PORTC & 0XF0) | ((seconds / 10) % 10);
	_delay_ms(2);
	PORTA = (PORTA & 0XC0) | (1 << PA2);
	PORTC = (PORTC & 0XF0) | (minutes % 10);
	_delay_ms(2);
	PORTA = (PORTA & 0XC0) | (1 << PA3);
	PORTC = (PORTC & 0XF0) | ((minutes / 10) % 10);
	_delay_ms(2);
	PORTA = (PORTA & 0XC0) | (1 << PA4);
	PORTC = (PORTC & 0XF0) | (hours % 10);
	_delay_ms(2);
	PORTA = (PORTA & 0XC0) | (1 << PA5);
	PORTC = (PORTC & 0XF0) | ((hours / 10) % 10);
	_delay_ms(2);

}

int main() {

	DDRC |= 0X0F;
	PORTC &= 0XF0;
	DDRA |= 0X3F;
	PORTA &= 0XC0;
	INT0_init();
	INT1_init();
	INT2_init();
	timer1_init();
	while (1) {
		display();
	}
}
