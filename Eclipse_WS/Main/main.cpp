

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
		METRONOME,
	} color_flicker = METRONOME;

	Color	foreground = 0xFFFFFF;
	Color	background = Color(Material::PURPLE, 70);

	uint32_t total_period = 100;
	uint8_t	 first_half	 =  30;
	uint8_t  beat_count  =  4;

	uint32_t fx_counter = 0;

	void update() {
		foreground.alpha = 120;
		background.alpha = 15;

		if(++fx_counter > total_period)
			fx_counter = 0;

		switch(color_flicker) {
		case TWO_TONE: {
			Color tgtColor = foreground;
			if((first_half*total_period)/255 < fx_counter)
				tgtColor = background;

			for(uint8_t i=0; i<WS2812_NUM; i++)
				current[i].merge_overlay(tgtColor);
		break;
		}

		case CLOCK: {
			uint8_t clock_pos  = (WS2812_NUM*fx_counter)/total_period;
			uint8_t	clock_time = (WS2812_NUM*255*fx_counter) / (total_period);

			for(uint8_t i=0; i<WS2812_NUM; i++) {
				if((i == clock_pos) && (clock_time < first_half))
					current[i].merge_overlay(foreground);
				else
					current[i].merge_overlay(background);
			}
		break;
		}

		case METRONOME: {
			uint8_t clock_pos  = (beat_count*fx_counter)/total_period;
			uint8_t	clock_time = (beat_count*255*fx_counter) / (total_period);

			for(uint8_t i=0; i<WS2812_NUM; i++) {
				if(clock_pos == 0 && (clock_time < first_half))
					current[i].merge_overlay(foreground);
				else if(i == 0 && (clock_time < first_half))
					current[i].merge_overlay(foreground);
				else
					current[i].merge_overlay(background);
			}

		break;
		}


		default: break;
		}

		for(uint8_t i=0; i<WS2812_NUM; i++)
			raw_leds[i] = current[i].getLEDValue();
		cli();
		ws2812_setleds(reinterpret_cast<cRGB*>(raw_leds), WS2812_NUM);
		sei();
	}
}


int main() {

	DDRB |= 1<<5 | 1<<4;

	while(true) {
		_delay_ms(40);
		LEDs::update();
	}

	return 0;
}
