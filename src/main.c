#include <pebble.h>
/*
#define MY_UUID { 0xB0, 0x06, 0x37, 0x19, 0xFF, 0x41, 0x43, 0x23, 0xAB, 0x87, 0x7F, 0x3C, 0x3F, 0x20, 0xB5, 0xF9 }
PBL_APP_INFO(MY_UUID,
             "HEISENBERG", "KSB Digital, Inc.",
             1, 0, 
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);
			 */

Window *my_window;
TextLayer *time_layer;
BitmapLayer *image_layer;

static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
	static char time_text[] = "00:00";
	char *time_format;
	
	if (clock_is_24h_style()) {
 		time_format = "%R";
 	} else {
    	time_format = "%I:%M";
    }
	strftime(time_text, sizeof(time_text), time_format, tick_time);
	
    // Kludge to handle lack of non-padded hour format string
    // for twelve hour clock.
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
      memmove(time_text, &time_text[1], sizeof(time_text) - 1);
    }
	text_layer_set_text(time_layer, time_text);
}

void handle_init(void) {

	my_window = window_create();
	window_stack_push(my_window, true);
	window_set_background_color(my_window, GColorWhite);
	
	// Note: This needs to be 'de-inited' in the application's de-init handler'
	image_layer = bitmap_layer_create(GRect(0, 0, 144, 128));
	bitmap_layer_set_bitmap(image_layer, gbitmap_create_with_resource(RESOURCE_ID_HEISENBERG));
	layer_add_child(window_get_root_layer(my_window), bitmap_layer_get_layer(image_layer));

	time_layer = text_layer_create(GRect(0, 168-40, 144, 40));
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_color(time_layer, GColorBlack);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_BOLD_32)));
	
   // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_minute_tick(current_time, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);

  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(time_layer));
}

void handle_deinit(void) {
	bitmap_layer_destroy(image_layer);
	text_layer_destroy(time_layer);
	window_destroy(my_window);
}

int main(void) {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}
