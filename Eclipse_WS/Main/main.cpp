

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>

#include "XasLibs/TIMER/Timer1.h"
#include "XasLibs/TIMER/Timer2.h"

#include "buzzer.h"

#include "light_handling.h"

#include "XasLibs/Communication/XIRR/xirr.h"

volatile uint16_t timer1_sec_presc = 0;
volatile uint16_t timer1_bzrupdate_presc = 0;
volatile uint16_t timer1_ledupdate_presc = 0;
volatile uint32_t global_counter   = 0;

ISR(TIMER1_COMPA_vect) {
	global_counter++;

	Communication::XIRR::TX::update();

	if(++timer1_sec_presc >= 2000) {
		timer1_sec_presc = 0;
	}

	if(++timer1_bzrupdate_presc >= 80) {
		timer1_bzrupdate_presc = 0;
		BZR::update();
	}


	if(timer1_ledupdate_presc < 78)
		timer1_ledupdate_presc++;
	else if(timer1_ledupdate_presc == 78) {
		sei();
		timer1_ledupdate_presc = 79;
		LEDs::precalculate();
		timer1_ledupdate_presc = 80;
	}
	else if(timer1_ledupdate_presc == 80 && Communication::XIRR::is_idle()) {
		timer1_ledupdate_presc = 0;
		sei();
		LEDs::push();
	}
}

ISR(TIMER1_COMPB_vect) {
	Communication::XIRR::RX::update();
}
ISR(TIMER1_CAPT_vect) {
	Communication::XIRR::RX::pin_capture();
}

const BZR::note_t on_notes[] = {
		{ .freq = 1300, .dur = 3},
		{ .freq = 1400, .dur = 3},
		{ .freq = 1700, .dur = 3},
};

const BZR::note_t stop_notes[] = {
		{.freq = 750, .dur = 5},
		{.freq = 0,	  .dur = 2},
};

const BZR::note_t start_notes[] = {
		{.freq = 1400, .dur = 3},
		{.freq = 0,	   .dur = 2},
};

const BZR::note_t att_notes[] = {
		{.freq = 1046, .dur = 3},
		{.freq = 800,  .dur = 3},
};

const BZR::note_t hitme_notes[] = {
		{.freq = 800, .dur = 1},
		{.freq = 1200, .dur = 1},
};

const BZR::note_t positive_notes[] = {
		{.freq = 1200, .dur = 3},
		{0, 2},
		{1200, 3},
		{1600, 6},
};

const BZR::note_t negative_notes[] = {
		{.freq = 1100, .dur = 3},
		{0, 2},
		{1100, 3},
		{800, 6},
};

void beep_on_data(const uint8_t *data, uint8_t length, uint8_t key) {
	if(key != 0)
		return;

	LEDs::set_preset(static_cast<LEDs::led_preset_t>(1 + (*data & 0b111)));
	BZR::play_sequence(positive_notes, 4);
}

void test_button(bool ok) {
	LEDs::set_preset(LEDs::BTN_PREP);
	BZR::play_sequence(att_notes, 2, 1);
	_delay_ms(3000);

	BZR::play_sequence(hitme_notes, 2, 3);
	LEDs::set_preset(LEDs::BTN_READY);
	_delay_ms(1500);
	LEDs::set_preset(ok ? LEDs::BTN_OK : LEDs::BTN_NOK);
	if(ok)
		BZR::play_sequence(positive_notes, 4);
	else
		BZR::play_sequence(negative_notes, 4);

	_delay_ms(5000);
}

int main() {

	DDRB |= 1<<5 | 1<<1;

	BZR::init();

	Communication::XIRR::init();
	Communication::XIRR::RX::process_data = beep_on_data;
	sei();

	LEDs::set_preset(LEDs::NO_CONN);

	_delay_ms(1000);

	BZR::play_sequence(on_notes, 3, 1);

	_delay_ms(1000);

	uint8_t testC = 0;
	while(true) {
		_delay_ms(1000);
		Communication::XIRR::TX::send(&testC, 1, 0);
		testC++;
	}

	while(true) {
		BZR::play_sequence(start_notes, 2, 1);
		LEDs::set_preset(LEDs::WORK_ACTIVE);
		_delay_ms(60000);

		LEDs::set_preset(LEDs::WORK_STOP);
		BZR::play_sequence(stop_notes, 2, 3);
		_delay_ms(3000);

		test_button(false);
		test_button(true);
	}

	return 0;
}
