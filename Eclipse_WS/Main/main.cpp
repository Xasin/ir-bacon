

#include <avr/io.h>
#include <util/delay.h>

extern "C" {
#include "light_ws2812.h"
}

cRGB leds[3] = {};

int main() {

	DDRB |= 1<<5 | 1<<4;

	while(true) {
		_delay_ms(100);
		PORTB ^= 1<<5;

		leds[0].r ^= 0xF;
		leds[2].g += 15;

		ws2812_setleds(leds, 3);

	}

	return 0;
}
