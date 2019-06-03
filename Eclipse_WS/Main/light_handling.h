/*
 * light_handling.h
 *
 *  Created on: 26 May 2019
 *      Author: xasin
 */

#ifndef LIGHT_HANDLING_H_
#define LIGHT_HANDLING_H_

#define WS2812_NUM 3

extern "C" {
#include "light_ws2812.h"
}
#include "Color.h"
using namespace Peripheral;

namespace LEDs {
	enum led_preset_t {
		OFF,
		NO_CONN,
		WORK_ACTIVE,
		WORK_ATTENTION,
		WORK_STOP,

		BTN_PREP,
		BTN_READY,
		BTN_OK,
		BTN_NOK,
	};

	void precalculate();
	void push();

	void set_preset(led_preset_t set);
}


#endif /* LIGHT_HANDLING_H_ */
