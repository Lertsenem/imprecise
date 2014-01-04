#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 40

static struct CommonWordsData
{
	TextLayer * date;
	TextLayer * time;
	Window * window;
	char date_buffer[BUFFER_SIZE];
	char time_buffer[BUFFER_SIZE];
} s_data;

static void update_date(struct tm* t) {
	fuzzy_date_to_words(t->tm_wday, t->tm_mday, t->tm_mon, s_data.date_buffer, BUFFER_SIZE);
	text_layer_set_text(s_data.date, s_data.date_buffer);
}

static void update_time(struct tm* t) {
	fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.time_buffer, BUFFER_SIZE);
	text_layer_set_text(s_data.time, s_data.time_buffer);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
{
	update_time(tick_time);
}

static void handle_hour_tick(struct tm *tick_time, TimeUnits units_changed)
{
	update_date(tick_time);
}

static void do_init(void) {
	s_data.window = window_create();
	const bool animated = true;
	window_stack_push(s_data.window, animated);

	window_set_background_color(s_data.window, GColorBlack);
	//GFont time_font = fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD);
	GFont time_font = fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
	GFont date_font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);

	Layer *root_layer = window_get_root_layer(s_data.window);
	GRect frame = layer_get_frame(root_layer);

	/* time layer */
	s_data.time = text_layer_create(GRect(0, 30, frame.size.w, frame.size.h - 30));
	text_layer_set_background_color(s_data.time, GColorBlack);
	text_layer_set_text_color(s_data.time, GColorWhite);
	text_layer_set_font(s_data.time, time_font);
	layer_add_child(root_layer, text_layer_get_layer(s_data.time));

	/* date layer */
	s_data.date = text_layer_create(GRect(0, 0, frame.size.w, 20));
	text_layer_set_background_color(s_data.date, GColorBlack);
	text_layer_set_text_color(s_data.date, GColorWhite);
	text_layer_set_font(s_data.date, date_font);
	text_layer_set_text_alignment(s_data.date, GTextAlignmentCenter);
	layer_add_child(root_layer, text_layer_get_layer(s_data.date));

	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	update_time(t);
	update_date(t);

	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
	tick_timer_service_subscribe(HOUR_UNIT, &handle_hour_tick);
}

static void do_deinit(void) {
	window_destroy(s_data.window);
	text_layer_destroy(s_data.time);
	text_layer_destroy(s_data.date);
}

int main(void) {
	do_init();
	app_event_loop();
	do_deinit();
}
