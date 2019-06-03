/*
 * buzzer.h
 *
 *  Created on: 31 May 2019
 *      Author: xasin
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "XasLibs/TIMER/Timer2.h"

namespace BZR {

struct note_t {
	uint16_t freq;
	uint8_t dur;
};

void play_sequence(const note_t *start, uint8_t num, uint8_t repeats = 0);
void update();
void init();
}


#endif /* BUZZER_H_ */
