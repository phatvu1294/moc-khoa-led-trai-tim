#define F_CPU 9600000ul
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "random.h"

void xuat_du_lieu (unsigned char  b) {
	unsigned char  j;

	for (j = 0; j < 8; j++) {
		PORTB = (b & (0x80 >> j)) ? PORTB | (1 << 0) : PORTB & ~(1 << 0);

		PORTB &= ~(1 << 3);
		PORTB |= (1 << 3);
	}
	PORTB &= ~(1 << 4);
	PORTB |= (1 << 4);
}

void chop_tat(unsigned char  b) {
	unsigned char  j;

	for (j = 0; j < 96; j++) {
		xuat_du_lieu(b);
		b = ~b;
		_delay_ms(80);
	}
}

void xep_dan(unsigned char  xuoi) {
	unsigned char  b, a, i, j;

	b = 0x00;

	for (i = 0; i < 9; i++) {
		a = b;

		for (j = 0; j < 9 - i; j++) {
			xuat_du_lieu(b);
			b = a;
			b |= (xuoi) ? 0x01 << (j - 1) : 0x80 >> (j - 1);
			_delay_ms(50);
		}
	}
}

void sang_dan_tat_dan(unsigned char  batdau, unsigned char  lanlap, unsigned char  sangdan, unsigned char  xuoi) {
	unsigned char  b, j;

	b = 0x00;

	for (j = 0; j < lanlap; j++) {
		xuat_du_lieu(sangdan ? b : ~b);

		b |= (xuoi) ? (batdau << (j - 1)) : (batdau >> (j - 1));

		_delay_ms(50);
	}
}

void dich_sang(unsigned char  xuoi) {
	unsigned char  b, j;

	b = 0x00;

	for (j = 0; j < 13; j++) {
		xuat_du_lieu(b);
		b = (xuoi) ? (0x01 << (j - 1)) | (0x01 << (j - 2)) | (0x01 << (j - 3))
		: (0x80 >> (j - 1)) | (0x80 >> (j - 2)) | (0x80 >> (j - 3));
		_delay_ms(50);
	}
}

ISR (INT0_vect) {
	unsigned char  x = random() % 4, j;

	if (x == 0) {
		xep_dan(1);
		xep_dan(0);
		xep_dan(1);
		xep_dan(0);
	}

	else if (x == 1) {
		for (j = 0; j < 4; j++) {
			sang_dan_tat_dan(0x01, 10, 1, 1);
			sang_dan_tat_dan(0x01, 10, 0, 1);
			sang_dan_tat_dan(0x80, 10, 1, 0);
			sang_dan_tat_dan(0x80, 10, 0, 0);
		}
	}

	else if (x ==2) {
		for (j = 0; j < 6; j++) {
			dich_sang(1);
			dich_sang(0);
		}
	}

	else if (x == 3) {
		chop_tat(0xFF);
	}

	xuat_du_lieu(0x00);
}

int main(void) {
	random_init(0xabcd);

	PORTB = 0x19;
	DDRB = 0x19;

	GIMSK |= (1 << INT0);

	sei();

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	while (1) {
		sleep_mode();
	}
}


