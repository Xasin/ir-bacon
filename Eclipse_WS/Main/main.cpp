

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>

#include "XasLibs/TIMER/Timer1.h"
#include "XasLibs/TIMER/Timer2.h"

#include "light_handling.h"

volatile uint16_t timer1_sec_presc = 0;
volatile uint16_t timer1_ledupdate_presc = 0;
volatile uint32_t global_counter   = 0;

namespace BZR {
void update();
}

ISR(TIMER1_COMPA_vect) {
	global_counter++;

	if(++timer1_sec_presc >= 4000) {
		timer1_sec_presc = 0;
		PORTB ^= 1<<5;
	}

	if(++timer1_ledupdate_presc >= 160) {
		timer1_ledupdate_presc = 0;
		LEDs::update();

		BZR::update();
	}
}

void test_button(bool ok) {
	LEDs::set_preset(LEDs::BTN_PREP);
	_delay_ms(3000);

	LEDs::set_preset(LEDs::BTN_READY);
	_delay_ms(1500);
	LEDs::set_preset(ok ? LEDs::BTN_OK : LEDs::BTN_NOK);
	_delay_ms(5000);
}

namespace BZR {

struct note_t {
	uint16_t freq;
	uint8_t dur;
};

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

void play_sequence(const note_t *start, uint8_t num, uint8_t repeats = 0) {
	startNote = start;
	note_count = num;

	sequence_repeats = repeats;

	remaining_note_ticks = start->dur;
	set_freq(start->freq);

	remaining_notes		 = num;
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

const BZR::note_t on_notes[] = {
		{ .freq = 1300, .dur = 2},
		{ .freq = 1400, .dur = 2},
		{ .freq = 1600, .dur = 2},
};

const BZR::note_t stop_notes[] = {
		{.freq = 800, .dur = 3},
		{.freq = 0,	  .dur = 2},
};

int main() {

	DDRB |= 1<<5 | 1;

	BZR::init();

	Timer1::enable_CTC(4000);
	sei();

	LEDs::set_preset(LEDs::NO_CONN);

	_delay_ms(1000);

	BZR::play_sequence(on_notes, 3, 1);

	_delay_ms(10000);

	while(true) {
		LEDs::set_preset(LEDs::WORK_ACTIVE);
		_delay_ms(10000);

		LEDs::set_preset(LEDs::WORK_STOP);
		BZR::play_sequence(stop_notes, 1, 2);
		_delay_ms(3000);

		test_button(false);
		test_button(true);
	}

	return 0;
}
