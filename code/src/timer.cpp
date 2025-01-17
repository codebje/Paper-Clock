#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

volatile uint_fast8_t timer_intrpt_flag = 0;

ISR(TIMER2_COMPA_vect, ISR_BLOCK) {
	timer_intrpt_flag = !(0);
}

void timer_enable(void) {
	cli();
	TCCR2A = 0xC2; // Set OC2A on Compare Match, CTC mode
	TCCR2B = 0x07; // clkT2S/1024
	OCR2A = TIMER_OC; // Count up to 250
	TIMSK2 = 0x02; // Only respond to OCF2A interrupt
	sei();
}

void timer_disable(void) {
	cli();
	TCCR2A = 0x00;
	TCCR2B = 0x00;
	TIMSK2 = 0x00;
	sei();
}

void timer_wait(uint16_t ticks) {
    cli();
    TCNT2 = 0;
    sei();
    cli();
    timer_intrpt_flag = 0;
    sei();
    uint16_t cnt = 0;
    while (cnt < ticks) {
        cli();
        set_sleep_mode(SLEEP_MODE_PWR_SAVE);
        sleep_enable();
  	    sei();
  	    sleep_cpu();
  	    sleep_disable(); 
        cli();
        if (timer_intrpt_flag) {
            timer_intrpt_flag = 0;
            sei();
            ++cnt;
        }
        sei();
    }
}
