

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>

extern "C" {
#include "light_ws2812.h"
}
#include "Color.h"
using namespace Peripheral;

#define WS2812_NUM 3

namespace LEDs {
	Color current[WS2812_NUM] = {};

	Color::ColorData raw_leds[WS2812_NUM] = {};

	enum color_flicker_t {
		TWO_TONE,
		D_FLICKER,
		CLOCK,
	} color_flicker;

	Color	foreground = Color();
	Color	background = Color();

	int		total_period = 20;
	uint8_t	first_half	 = 127;
	uint8_t	effect_alpha = 50;

	int fx_counter = 0;

	void update() {
		if(++fx_counter > total_period)
			fx_counter = 0;

		switch(color_flicker) {
		case TWO_TONE: {
			Color tgtColor = foreground;
			if((first_half*fx_counter)/255 > total_period)
				tgtColor = background;

			for(uint8_t i=0; i<WS2812_NUM; i++)
				current[i].merge_overlay(tgtColor, effect_alpha);
		break;
		}
		}

		for(uint8_t i=0; i<WS2812_NUM; i++) {
			raw_leds[i] = current[i].getLEDValue();
		}
		cli();
		ws2812_setleds(reinterpret_cast<cRGB*>(raw_leds), WS2812_NUM);
		sei();
	}
}


int main() {

	DDRB |= 1<<5 | 1<<4;

	while(true) {
		_delay_ms(50);



		LEDs::update();
	}

	return 0;
}
