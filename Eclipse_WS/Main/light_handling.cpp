/*
 * light_handling.cpp
 *
 *  Created on: 26 May 2019
 *      Author: xasin
 */

#include "light_handling.h"


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
		if(++fx_counter >= total_period)
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

	void set_preset(led_preset_t set) {
		fx_counter = 0;

		switch(set) {
		case OFF:
			foreground = 0;
			background = 0;
		break;

		case NO_CONN:
			foreground = Color(Material::PURPLE, 100);
			background = 0;

			foreground.alpha = 40;
			background.alpha = 10;

			color_flicker = CLOCK;
			total_period  = 25*WS2812_NUM;
			first_half 	  = 127;
		break;

		case WORK_ACTIVE:
			foreground = Color(Material::GREEN, 130);
			background = Color(Material::GREEN, 30);

			foreground.alpha = 25;
			background.alpha = 20;

			color_flicker = CLOCK;
			total_period  = 25*WS2812_NUM;
			first_half	  = 255;
		break;

		case WORK_ATTENTION:
			foreground = Material::AMBER;
			foreground.alpha = 180;
			background = Color(Material::YELLOW, 30);
			background.alpha = 100;

			color_flicker = CLOCK;
			total_period  = 25;
			first_half 	  = 255;
		break;

		case WORK_STOP:
			foreground = Material::RED;
			foreground.alpha = 80;
			background = Color(Material::RED, 80);
			background.alpha = 50;

			color_flicker = TWO_TONE;
			total_period  = 1.5*25;
			first_half 	  = 100;
		break;


		case BTN_PREP:
			foreground = Material::ORANGE;
			foreground.alpha = 100;
			background = 0x404044;
			background.alpha = 50;

			color_flicker = CLOCK;
			total_period  = 21;
			first_half    = 200;
		break;

		case BTN_READY:
			foreground = Material::CYAN;
			foreground.alpha = 230;
			background = 0x545050;
			background.alpha = 150;

			color_flicker = CLOCK;
			total_period  = 9;
			first_half    = 255;
		break;

		case BTN_OK:
			foreground = Material::GREEN;
			foreground.alpha = 160;
			background = 0x565056;
			background.alpha = 60;

			color_flicker = CLOCK;
			total_period  = 15;
			first_half    = 200;
		break;

		case BTN_NOK:
			foreground = Material::DEEP_ORANGE;
			foreground.alpha = 160;
			background = 0x908080;
			background.alpha = 50;

			color_flicker = CLOCK;
			total_period  = 21;
			first_half    = 200;
		break;
		}
	}
}
