/*
 * NOTE: This is an older version of some code to send pins to the Timeline.
 * I am currently writing a Timeline pin library, and so I am not updating
 * this, as it will be replaced.
 */


#include <pebble.h>
#include "phone.h"
#include "countdown_timer.h"

// AppMessage Keys
#define KEY_DURATION 5
#define KEY_UNIQUEID 10
#define KEY_TOTAL_TIME 15
#define KEY_START_TIMER 20

// set once at connect; there is only ever one of these
static PhoneStartTimerHandler s_start_timer_handler = NULL;



// ********** Utilities ********** //
// send a pin for the timer
void phone_send_pin(CountdownTimer *countdown_timer) {
  // begin iterator
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  // write data
  dict_write_int32(iter, KEY_UNIQUEID, countdown_timer_get_id(countdown_timer));
  dict_write_int32(iter, KEY_DURATION, countdown_timer_get_current_time(countdown_timer) / 1000);
  dict_write_int32(iter, KEY_TOTAL_TIME, countdown_timer_get_duration(countdown_timer) / 1000);
  dict_write_end(iter);
  // send
  app_message_outbox_send();
}
// delete a pin
void phone_delete_pin(CountdownTimer *countdown_timer) {
  // begin iterator
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  // write data (duration of 0 means delete pin)
  dict_write_int32(iter, KEY_UNIQUEID, countdown_timer_get_id(countdown_timer));
  dict_write_int32(iter, KEY_DURATION, 0);
  dict_write_int32(iter, KEY_TOTAL_TIME, 0);
  dict_write_end(iter);
  // send
  app_message_outbox_send();
}



// ********** Callbacks ********** //
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
// a message arrived from the phone
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *start_timer = dict_find(iterator, KEY_START_TIMER);
  if (start_timer == NULL || s_start_timer_handler == NULL) {
    return;
  }
  s_start_timer_handler(start_timer->value->int32);
}
// error callbacks
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason,
                                   void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed: %d", (APP_MSG_NOT_CONNECTED == (int)reason));
}



// ********** Initialize/De-initialize ********** //
// start phone connection
void phone_connect(PhoneStartTimerHandler start_timer_handler) {
  s_start_timer_handler = start_timer_handler;
  // register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  // Open AppMessage
  app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
}

// stop phone connection
void phone_disconnect(void) {
  // unregister callbacks
  app_message_deregister_callbacks();
  s_start_timer_handler = NULL;
}
