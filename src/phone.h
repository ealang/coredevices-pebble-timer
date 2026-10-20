/*
 * NOTE: This is an older version of some code to send pins to the Timeline.
 * I am currently writing a Timeline pin library, and so I am not updating
 * this, as it will be replaced.
 */


#pragma once

#include <pebble.h>
#include "countdown_timer.h"

/*
 * Callback:    PhoneStartTimerHandler
 * -----------------------------------
 * called when the phone asks for a new timer to be created and started
 *
 *  seconds: the requested duration, straight off the wire and not yet validated
 */

typedef void (*PhoneStartTimerHandler)(int32_t seconds);

// public functions
void phone_send_pin(CountdownTimer *countdown_timer);
void phone_delete_pin(CountdownTimer *countdown_timer);
void phone_connect(PhoneStartTimerHandler start_timer_handler);
void phone_disconnect(void);
