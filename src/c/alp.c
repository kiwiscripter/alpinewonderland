#include <pebble.h>

static Window *s_window;
static TextLayer *s_text_layer;
static GBitmap *s_bitmap;
static BitmapLayer *s_bitmap_layer;
static TextLayer *s_subtext_layer;
static TextLayer *s_supertext_layer;


// time handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  static char s_buffer[32];
  // new buffer for subtext
  static char s_subtext_buffer[32];

  // new buffer for supertext
  static char s_supertext_buffer[32];
  // change subtext to long day and day of month and month (3 letter like NOV for November)
  strftime(s_subtext_buffer, sizeof(s_subtext_buffer), "%A %d, %b", tick_time);
  
  // check if hour is 12 or 24
  if (clock_is_24h_style()) {
    strftime(s_buffer, sizeof(s_buffer), "%H:%M", tick_time);
  } else {
    strftime(s_buffer, sizeof(s_buffer), "%I:%M", tick_time);

    // remove leading zero from hour
    if (s_buffer[0] == '0') {
      memmove(s_buffer, &s_buffer[1], sizeof(s_buffer) - 1);
    }

    //change subtext to be am or pm
    strftime(s_supertext_buffer, sizeof(s_supertext_buffer), "%p", tick_time);

  }
  // make sure the text layer is able to display the time
  text_layer_set_text(s_text_layer, s_buffer);
  // set subtext layer
  text_layer_set_text(s_subtext_layer, s_subtext_buffer);
  // set supertext layer
  text_layer_set_text(s_supertext_layer, s_supertext_buffer);

}

static void prv_window_load(Window *window)
{

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  BitmapLayer *s_bitmap_layer = bitmap_layer_create(bounds);

  // right padding offset
  int offset = -4;

  
  if (gbitmap_get_format(s_bitmap) == GBitmapFormat1Bit) {
    bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  } else {
    bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpAssign);
  }
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));

  // create text
  s_text_layer = text_layer_create(GRect(offset+1, 120, bounds.size.w, bounds.size.h));
  text_layer_set_background_color(s_text_layer, GColorClear);
  //if the pebble is color, set the text color to white
  if (gbitmap_get_format(s_bitmap) == GBitmapFormat1Bit) {
    text_layer_set_text_color(s_text_layer, GColorBlack);
  } else {
    text_layer_set_text_color(s_text_layer, GColorWhite);
  }
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  // create subtext
  s_subtext_layer = text_layer_create(GRect(offset, 147, bounds.size.w, bounds.size.h));
  text_layer_set_background_color(s_subtext_layer, GColorClear);
  //if the pebble is color, set the text color to white
  if (gbitmap_get_format(s_bitmap) == GBitmapFormat1Bit) {
    text_layer_set_text_color(s_subtext_layer, GColorBlack);
  } else {
    text_layer_set_text_color(s_subtext_layer, GColorWhite);
  }
  text_layer_set_font(s_subtext_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_subtext_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_subtext_layer));

  // create supertext
  s_supertext_layer = text_layer_create(GRect(offset, 109, bounds.size.w, bounds.size.h));
  text_layer_set_background_color(s_supertext_layer, GColorClear);
  //if the pebble is color, set the text color to white
  if (gbitmap_get_format(s_bitmap) == GBitmapFormat1Bit) {
    text_layer_set_text_color(s_supertext_layer, GColorBlack);
  } else {
    text_layer_set_text_color(s_supertext_layer, GColorWhite);
  }
  text_layer_set_font(s_supertext_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_supertext_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_supertext_layer));

  // make sure the time is displayed from the start
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  tick_handler(current_time, MINUTE_UNIT);
}

static void prv_window_unload(Window *window)
{
  text_layer_destroy(s_text_layer);
  gbitmap_destroy(s_bitmap);
  bitmap_layer_destroy(s_bitmap_layer);

}

static void prv_init(void)
{
  s_window = window_create();

  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });

  // time handler
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);


  
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void prv_deinit(void)
{
  window_destroy(s_window);
}

int main(void)
{
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
