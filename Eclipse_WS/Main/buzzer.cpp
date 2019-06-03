/*
 * buzzer.cpp
 *
 *  Created on: 31 May 2019
 *      Author: xasin
 */


#include "buzzer.h"

namespace BZR {

void set_freq(uint16_t freq) {
	if(freq == 0) {
		Timer2::set_OC2B_mode(0);
		DDRD &= ~(1<<3);
	}
	else {
		Timer2::set_OC2B_mode(TIMER2_OC2B_NPWM_TOGGLE);
		DDRD |= 1<<3;
	}

	Timer2::set_OC2A_frequency(freq);
}

void init() {
	Timer2::set_mode(TIMER2_MODE_TOP_OCR2A);
	Timer2::set_OC2B_mode(TIMER2_OC2B_NPWM_TOGGLE);
}

uint16_t sequence_repeats = 0;
uint16_t note_count = 0;
const note_t *startNote = nullptr;

volatile uint16_t remaining_note_ticks = 0;
volatile uint16_t remaining_notes = 0;

const note_t *nextNote = nullptr;

void play_sequence(const note_t *start, uint8_t num, uint8_t repeats) {
	startNote = start;
	note_count = num;

	sequence_repeats = repeats;

	remaining_note_ticks = start->dur;
	set_freq(start->freq);

	remaining_notes		 = num-1;
	nextNote 			 = start;
}

void update() {
	if(remaining_note_ticks > 0)
		remaining_note_ticks--;
	else if(remaining_notes > 0) {
		nextNote++;
		remaining_notes--;

		set_freq(nextNote->freq);
		remaining_note_ticks = nextNote->dur;
	}
	else if(sequence_repeats > 0) {
		play_sequence(startNote, note_count, sequence_repeats-1);
	}
	else
		set_freq(0);
}

}
