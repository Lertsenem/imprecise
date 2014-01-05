#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 40

#define UI_BATTERY_LINE_X 20
#define UI_BLUETOOTH_X 5

/* STRUCTURES */
/* ======================================== */
static struct CommonWordsData
{
	TextLayer * date;
	TextLayer * time;
	Layer * battery;
	Layer * bluetooth;
	Layer * static_ui;
	Window * window;
	char date_buffer[BUFFER_SIZE];
	char time_buffer[BUFFER_SIZE];
} s_data;


/* FUNCTIONS */
/* ======================================== */

/* Updaters */
/* -------- */
static void update_date(struct tm* t) {
	fuzzy_date_to_words(t->tm_wday, t->tm_mday, t->tm_mon, s_data.date_buffer, BUFFER_SIZE);
	text_layer_set_text(s_data.date, s_data.date_buffer);
}

static void update_time(struct tm* t) {
	fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.time_buffer, BUFFER_SIZE);
	text_layer_set_text(s_data.time, s_data.time_buffer);
}

/*
 * Hopefully, called once only, at start.
 */
static void static_ui_update_callback(Layer * me, GContext * ctx)
{
	/* set bg color to black */
	graphics_context_set_fill_color(ctx, GColorBlack);

	/* battery extremities markers */
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_line(ctx, GPoint(UI_BATTERY_LINE_X -1, 1), GPoint(UI_BATTERY_LINE_X -1, 2));
	graphics_draw_line(ctx, GPoint(UI_BATTERY_LINE_X +101, 1), GPoint(UI_BATTERY_LINE_X +101, 2));
}

static void battery_update_callback(Layer * me, GContext * ctx)
{
	/* set bg color to black */
	graphics_context_set_fill_color(ctx, GColorBlack);

	/* get battery */
	BatteryChargeState battery = battery_state_service_peek();

	/* draw battery as a line, add dots if charging */
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_line(ctx, GPoint(0, 0), GPoint(battery.charge_percent, 0));
	graphics_draw_pixel(ctx, GPoint(50, 0)); /* mark the middle */
	
	if (battery.is_charging && battery.charge_percent < 100)
	{
		graphics_context_set_stroke_color(ctx, GColorBlack);
		graphics_draw_pixel(ctx, GPoint(battery.charge_percent -1, 0));
		graphics_draw_pixel(ctx, GPoint(battery.charge_percent -3, 0));
		graphics_draw_pixel(ctx, GPoint(battery.charge_percent -5, 0));
	}
}

static void bluetooth_update_callback(Layer * me, GContext * ctx)
{
	/* set bg color to black */
	graphics_context_set_fill_color(ctx, GColorBlack);

	/* get bluetooth */
	bool connected = bluetooth_connection_service_peek();

	/* draw a cross if no bluetooth connexion */
	if(!connected)
	{
		graphics_context_set_stroke_color(ctx, GColorWhite);
		graphics_draw_pixel(ctx, GPoint(0, 0));
		graphics_draw_pixel(ctx, GPoint(3, 0));
		graphics_draw_pixel(ctx, GPoint(1, 1));
		graphics_draw_pixel(ctx, GPoint(2, 1));
		graphics_draw_pixel(ctx, GPoint(1, 2));
		graphics_draw_pixel(ctx, GPoint(2, 2));
		graphics_draw_pixel(ctx, GPoint(0, 3));
		graphics_draw_pixel(ctx, GPoint(3, 3));
	}
}

/* Handles */
/* ------- */
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
{
	update_date(tick_time);
	update_time(tick_time);
}

static void handle_battery(BatteryChargeState charge_state)
{
	/* mark the battery layer dirty (ask for redraw) */
	layer_mark_dirty(s_data.battery);
}

static void handle_bluetooth(bool connected)
{
	/* mark the bluetooth layer dirty (ask for redraw) */
	layer_mark_dirty(s_data.bluetooth);
}

/* Doers */
/* ----- */
static void do_init(void) {
	s_data.window = window_create();
	const bool animated = true;
	window_stack_push(s_data.window, animated);

	window_set_background_color(s_data.window, GColorBlack);

	/* fonts */
	GFont time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GOTHAM_CITY_30));
	GFont date_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);

	/* root layer */
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

	/* static_ui layer (the statics elements of the ui) */
	s_data.static_ui = layer_create(GRect(0,frame.size.h-5,frame.size.w,frame.size.h));
	layer_set_update_proc(s_data.static_ui, static_ui_update_callback);
	layer_add_child(root_layer, s_data.static_ui);
	layer_mark_dirty(s_data.static_ui);

	/* battery layer */
	s_data.battery = layer_create(GRect(UI_BATTERY_LINE_X,2,UI_BATTERY_LINE_X+100,2));
	layer_set_update_proc(s_data.battery, battery_update_callback);
	layer_add_child(s_data.static_ui, s_data.battery);
	layer_mark_dirty(s_data.battery);

	/* bluetooth layer */
	s_data.bluetooth = layer_create(GRect(UI_BLUETOOTH_X,0,UI_BLUETOOTH_X+2,4));
	layer_set_update_proc(s_data.bluetooth, bluetooth_update_callback);
	layer_add_child(s_data.static_ui, s_data.bluetooth);
	layer_mark_dirty(s_data.bluetooth);

	/* first run */
	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	update_time(t);
	update_date(t);

	/* suscribtions */
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
	battery_state_service_subscribe(&handle_battery);
	bluetooth_connection_service_subscribe(&handle_bluetooth);
}

static void do_deinit(void) {
	window_destroy(s_data.window);
	text_layer_destroy(s_data.time);
	text_layer_destroy(s_data.date);
	layer_destroy(s_data.battery);
	layer_destroy(s_data.bluetooth);
	layer_destroy(s_data.static_ui);
}

int main(void) {
	do_init();
	app_event_loop();
	do_deinit();
}
