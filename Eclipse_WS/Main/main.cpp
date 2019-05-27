

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>

#include "XasLibs/TIMER/Timer1.h"

#include "light_handling.h"

volatile uint16_t timer1_sec_presc = 0;
volatile uint16_t timer1_ledupdate_presc = 0;
volatile uint32_t global_counter   = 0;

ISR(TIMER1_COMPA_vect) {
	global_counter++;

	if(++timer1_sec_presc >= 4000) {
		timer1_sec_presc = 0;
		PORTB ^= 1<<5;
	}

	if(++timer1_ledupdate_presc >= 160) {
		timer1_ledupdate_presc = 0;
		LEDs::update();
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

int main() {

	DDRB |= 1<<5 | 1;

	Timer1::enable_CTC(4000);

	sei();

	LEDs::set_preset(LEDs::NO_CONN);

	_delay_ms(10000);

	while(true) {
		LEDs::set_preset(LEDs::WORK_ACTIVE);
		_delay_ms(10000);

		LEDs::set_preset(LEDs::WORK_STOP);
		_delay_ms(3000);

		test_button(false);
		test_button(true);
	}

	return 0;
}
