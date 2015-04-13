#include <pebble.h>

Window *window;
TextLayer *text_layer;
GFont *font;
AppTimer *timer;

char text[300];
int time_ticks = 0;
int state = 0;
int interval = 150;
char cursor = 0;

void draw_text();

void app_timer(void * data)
{
  time_ticks++;
  draw_text();
}

void draw_text()
{
  time_t t = time(NULL);
  struct tm * tick_time = localtime(&t);  
  snprintf(text, sizeof(text), "C:\\>time /t\n%02d:%02d\n\nC:\\>date /t\n%02d.%02d.%04d", tick_time->tm_hour, tick_time->tm_min, tick_time->tm_mday, tick_time->tm_mon+1, tick_time->tm_year+1900);
  
  if (time_ticks >= 2)
  {
    state++;
    time_ticks = 0;
  }
  
  cursor ^= 1;
  
  if (state <= 6)
  {
    text[4+state] = cursor ? '_' : ' ';
    text[5+state] = 0;
  }
  else if (state < 14)
  {
    text[16+state] = cursor ? '_' : ' ';
    text[17+state] = 0;
  } else timer = NULL;
  if (state < 14)
  {    
    timer = app_timer_register(interval, app_timer, NULL);    
  }
  text_layer_set_text(text_layer, text);
}

void start_draw()
{
  time_ticks = state = 0;
  draw_text();
}

void tick(struct tm *tick_time, TimeUnits units_changed)
{
  //if (!timer)
    start_draw();
}

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
  window_set_background_color(window, GColorBlack);
	text_layer = text_layer_create(GRect(2, -5, 144, 168));
  
  font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TERMINAL_32));
	
	// Set the text, font, and text alignment
	text_layer_set_font(text_layer, font);
	text_layer_set_text_alignment(text_layer, GTextAlignmentLeft);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text(text_layer, "C:\\>");
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));

	// Push the window
	window_stack_push(window, true);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick);
	
  //start_draw();
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
}

void handle_deinit(void) {
  // Stop timers
  tick_timer_service_unsubscribe();  
  if (timer) app_timer_cancel(timer);
  
	// Destroy the text layer
	text_layer_destroy(text_layer);
	
	// Destroy the window
	window_destroy(window);
  
  // Unload font
  fonts_unload_custom_font(font);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
