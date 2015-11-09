#include <pebble.h>

#define KEY_BACKGROUND_COLOR 0
#define KEY_FOREGROUND_COLOR 1

static Window *window;
static Layer *s_layer;

static Layer *s_bluetooth_icon_layer;
static bool s_bluetooth_connected;

static uint8_t s_hour;
static uint8_t s_min;
static uint8_t s_sec;

static int PADDING = 12;
static int HEIGHT = 25;

static GBitmap *s_ternary_minutes_bitmap;
static BitmapLayer *s_ternary_minutes_layer;
static GColor background_color;
static GColor foreground_color;

static const GPathInfo ONE_INFO = {
	.num_points = 6,
	.points = (GPoint []) {{54,32},{68,46},{51,63},{37,49},{37,48},{53,32}}
};

static GPath *one_path = NULL;

static const GPathInfo TWO_INFO = {
	.num_points = 6,
	.points = (GPoint []) {{89,32},{105,48},{105,49},{91,63},{74,46},{88,32}}
};

static GPath *two_path = NULL;

static const GPathInfo THREE_INFO = {
	.num_points = 6,
	.points = (GPoint []) {{91,104},{105,118},{105,119},{89,135},{88,135},{74,121}}
};

static GPath *three_path = NULL;

static const GPathInfo FOUR_INFO = {
	.num_points = 6,
	.points = (GPoint []) {{51,104},{68,121},{54,135},{53,135},{37,119},{37,118}}
};

static GPath *four_path = NULL;

static const GPathInfo FIVE_INFO = {
	.num_points = 8,
	.points = (GPoint []) {{40,18},{53,31},{53,32},{37,48},{36,48},{23,35},{23,34},{39,18}}
};

static GPath *five_path = NULL;

static const GPathInfo SIX_INFO = {
	.num_points = 8,
	.points = (GPoint []) {{103,18},{119,34},{119,35},{106,48},{105,48},{89,32},{89,31},{102,18}}
};

static GPath *six_path = NULL;

static GPathInfo SEVEN_INFO = {
	.num_points = 8,
	.points = (GPoint []) {{106,119},{119,132},{119,133},{103,149},{102,149},{89,136},{89,135},{105,119}}
};

static GPath *seven_path = NULL;

static GPathInfo EIGHT_INFO = {
	.num_points = 8,
	.points = (GPoint []) {{37,119},{53,135},{53,136},{40,149},{39,149},{23,133},{23,132},{36,119}}
};

static GPath *eight_path = NULL;

static GPathInfo NINE_INFO = {
	.num_points = 6,
	.points = (GPoint []) {{25,3},{39,17},{39,18},{23,34},{22,34},{8,20}}
};

static GPath *nine_path = NULL;

static GPathInfo TEN_INFO = {
	.num_points = 6,
	.points = (GPoint []) {{117,3},{134,20},{120,34},{119,34},{103,18},{103,17}}
};

static GPath *ten_path = NULL;

static GPathInfo ELEVEN_INFO = {
	.num_points = 6,
	.points = (GPoint []) {{120, 133},{134,147},{117,164},{103,150},{103,149},{119,133}}
};

static GPath *eleven_path = NULL;

static GPathInfo TWELVE_INFO = {
	.num_points = 6,
	.points = (GPoint []) {{23,133},{39,149},{39,150},{25,164},{8,147},{22,133}}
};

static GPath *twelve_path = NULL;

static const GPathInfo BLUETOOTH_INFO = {
	.num_points = 9,
	.points = (GPoint []) {{3,26},{26,3},{14,15},{14,26},{20,20},{9,9},{14,14},{14,3},{20,9}}
};

GPath *bluetooth_path = NULL;

static void bluetooth_callback(bool connected) {
	
	//show icon if disconnected
	if (!connected) {
		//issue a vibrating alert
		vibes_double_pulse();
	}
	
	s_bluetooth_connected = connected;
	layer_mark_dirty(s_bluetooth_icon_layer);
}

static void bluetooth_update_proc(Layer *layer, GContext *ctx) {
	if (!s_bluetooth_connected) {
		graphics_context_set_stroke_width(ctx, 3);
		graphics_context_set_stroke_color(ctx, gcolor_legible_over(background_color));
		gpath_draw_outline(ctx, bluetooth_path);
	}
}

static void update_time(struct tm *tick_time) {
	s_hour = tick_time->tm_hour;
	s_min = tick_time->tm_min;
	s_sec = tick_time->tm_sec;
	layer_mark_dirty(s_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time(tick_time);
}

static void draw_watchface(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	uint16_t width = bounds.size.w - (2 * PADDING);
	uint16_t max_height = bounds.size.h - (2 * PADDING);

	//set the colour
	graphics_context_set_fill_color(ctx, COLOR_FALLBACK(foreground_color, GColorRed));

	//display minutes
	graphics_fill_rect(ctx, GRect(((s_min * width) / 60) + PADDING, 84 - 10, 2, 19), 0, 0);

	//display hours
	
	uint8_t cur_time = s_hour % 12;
	if (cur_time == 0) {
		cur_time = 12;
	}

	switch (cur_time) {
		case 12:
			gpath_draw_filled(ctx, twelve_path);
		case 11:
			gpath_draw_filled(ctx, eleven_path);
		case 10:
			gpath_draw_filled(ctx, ten_path);
		case 9:
			gpath_draw_filled(ctx, nine_path);
		case 8:
			gpath_draw_filled(ctx, eight_path);
		case 7:
			gpath_draw_filled(ctx, seven_path);
		case 6:
			gpath_draw_filled(ctx, six_path);
		case 5:
			gpath_draw_filled(ctx, five_path);
		case 4:
			gpath_draw_filled(ctx, four_path);
		case 3:
			gpath_draw_filled(ctx, three_path);
		case 2:
			gpath_draw_filled(ctx, two_path);
		case 1:
			gpath_draw_filled(ctx, one_path);
			break;
		default:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Invalid Hour %d", cur_time);
	}

	graphics_context_set_stroke_color(ctx, gcolor_legible_over(background_color));
	graphics_context_set_stroke_width(ctx, 1);
	gpath_draw_outline(ctx, twelve_path);
	gpath_draw_outline(ctx, eleven_path);
	gpath_draw_outline(ctx, ten_path);
	gpath_draw_outline(ctx, nine_path);
	gpath_draw_outline(ctx, eight_path);
	gpath_draw_outline(ctx, seven_path);
	gpath_draw_outline(ctx, six_path);
	gpath_draw_outline(ctx, five_path);
	gpath_draw_outline(ctx, four_path);
	gpath_draw_outline(ctx, three_path);
	gpath_draw_outline(ctx, two_path);
	gpath_draw_outline(ctx, one_path);
}

static void setup_paths() {
	one_path = gpath_create(&ONE_INFO);
	two_path = gpath_create(&TWO_INFO);
	three_path = gpath_create(&THREE_INFO);
	four_path = gpath_create(&FOUR_INFO);
	five_path = gpath_create(&FIVE_INFO);
	six_path = gpath_create(&SIX_INFO);
	seven_path = gpath_create(&SEVEN_INFO);
	eight_path = gpath_create(&EIGHT_INFO);
	nine_path = gpath_create(&NINE_INFO);
	ten_path = gpath_create(&TEN_INFO);
	eleven_path = gpath_create(&ELEVEN_INFO);
	twelve_path = gpath_create(&TWELVE_INFO);
}

static void inbox_receieved_handler(DictionaryIterator *iter, void *context) {
	Tuple *background_color_t = dict_find(iter, KEY_BACKGROUND_COLOR);
	Tuple *foreground_color_t = dict_find(iter, KEY_FOREGROUND_COLOR);

	if (background_color_t) {
		int bc = background_color_t->value->int32;
		if (bc == 0) { //quick fix so that black colour persists
			bc++;
		}
		persist_write_int(KEY_BACKGROUND_COLOR, bc);
		background_color = GColorFromHEX(bc);
		window_set_background_color(window, background_color);
	}

	if (foreground_color_t) {
		int fc = foreground_color_t->value->int32;
		if (fc == 0) { //quick fix so that black colour persists
			fc++;
		}
		persist_write_int(KEY_FOREGROUND_COLOR, fc);
		foreground_color = GColorFromHEX(fc);
	}

  	//display the updates right away
	time_t start_time = time(NULL);
  	update_time(localtime(&start_time));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_layer = layer_create(layer_get_bounds(window_get_root_layer(window)));
  layer_add_child(window_get_root_layer(window), s_layer);
  layer_set_update_proc(s_layer, draw_watchface);

  uint16_t width = bounds.size.w - (2 * PADDING);
  uint16_t max_height = bounds.size.h - (2 * PADDING);

  //create the ternary minutes image
  s_ternary_minutes_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MINUTES_TERNARY_REFLECT);
  s_ternary_minutes_layer = bitmap_layer_create(GRect(PADDING, 0, width + 1, 168));
  bitmap_layer_set_background_color(s_ternary_minutes_layer, GColorClear);
  bitmap_layer_set_bitmap(s_ternary_minutes_layer, s_ternary_minutes_bitmap);
  bitmap_layer_set_compositing_mode(s_ternary_minutes_layer, GCompOpSet);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_ternary_minutes_layer));

  setup_paths();

  if (persist_read_int(KEY_BACKGROUND_COLOR)) {
	  background_color = GColorFromHEX(persist_read_int(KEY_BACKGROUND_COLOR));
	  window_set_background_color(window, background_color);
  } else {
	  background_color = GColorWhite;
	  window_set_background_color(window, background_color);
  }

  if (persist_read_int(KEY_FOREGROUND_COLOR)) {
	  foreground_color = GColorFromHEX(persist_read_int(KEY_FOREGROUND_COLOR));
  } else {
	  foreground_color = GColorRed;
  }

  s_bluetooth_icon_layer = layer_create(GRect(57,0,30,30));
  layer_set_update_proc(s_bluetooth_icon_layer, bluetooth_update_proc);
  bluetooth_path = gpath_create(&BLUETOOTH_INFO);
  layer_add_child(window_get_root_layer(window), s_bluetooth_icon_layer);

  //show the correct state of the bluetooth connection from the start
#ifdef PBL_SDK_2
  bluetooth_callback(bluetooth_connection_service_peek());
#elif PBL_SDK_3
  bluetooth_callback(connection_service_peek_pebble_app_connection());
#endif

}

static void window_unload(Window *window) {
	gbitmap_destroy(s_ternary_minutes_bitmap);
	bitmap_layer_destroy(s_ternary_minutes_layer);

	//destroy the main layer
	layer_destroy(s_layer);

	//destroy the bluetooth stuffs
	layer_destroy(s_bluetooth_icon_layer);
	gpath_destroy(bluetooth_path);

	gpath_destroy(one_path);
	gpath_destroy(two_path);
	gpath_destroy(three_path);
	gpath_destroy(four_path);
	gpath_destroy(five_path);
	gpath_destroy(six_path);
	gpath_destroy(seven_path);
	gpath_destroy(eight_path);
	gpath_destroy(nine_path);
	gpath_destroy(ten_path);
	gpath_destroy(eleven_path);
	gpath_destroy(twelve_path);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  //Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  app_message_register_inbox_received(inbox_receieved_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  //Register for Bluetooth connections updates
#ifdef PBL_SDK_2
  bluetooth_connection_service_subscribe(bluetooth_callback);
#elif PBL_SDK_3
  connection_service_subscribe((ConnectionHandlers) {
		  .pebble_app_connection_handler = bluetooth_callback
		  });
#endif

  //display the time right away
  time_t start_time = time(NULL);
  update_time(localtime(&start_time));
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
