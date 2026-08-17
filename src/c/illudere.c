#include <pebble.h>

// Illudere draws the time as four big digits hidden in a field of diagonal
// stripes. The background stripes run one way, the stripes inside the digits
// run the other, so a digit never appears as an outline -- only as the corners
// where the two directions meet. Follow a corner and the rest of the edge
// turns up.
//
// Everything is laid out in "cells": one cell is one pixel of the bitmap font,
// blown up to a square of `tile` screen pixels. The cell size is picked at
// runtime from the size of the screen, so the same code fills a 144x168
// Pebble 2 Duo and a 260x260 Pebble Round 2.

// Show minutes and seconds instead of hours and minutes, so you don't have to
// wait an hour to see the display change.
#define DEBUG false

// The stripe profile, sampled across the width of one cell. 1 is ink.
static const uint8_t LINE[] = {0, 0, 1, 1, 1, 1, 1, 0, 0, 0};
#define LINE_N ((int)(sizeof LINE / sizeof *LINE))

#define FONT font_thin

// Blank cells between the two digits of a row, and between the two rows.
#define SPACING_X 1
#define SPACING_Y 2

// Two fonts to pick between; whichever FONT isn't set to goes unreferenced.
#define OPTIONAL __attribute__((unused))

OPTIONAL static const uint8_t font_thick[][5][3] = {{
  {1, 1, 1},
  {1, 0, 1},
  {1, 0, 1},
  {1, 0, 1},
  {1, 1, 1}
}, {
  {1, 1, 0},
  {0, 1, 0},
  {0, 1, 0},
  {0, 1, 0},
  {1, 1, 1}
}, {
  {1, 1, 1},
  {0, 0, 1},
  {1, 1, 1},
  {1, 0, 0},
  {1, 1, 1}
}, {
  {1, 1, 1},
  {0, 0, 1},
  {1, 1, 1},
  {0, 0, 1},
  {1, 1, 1}
}, {
  {1, 0, 1},
  {1, 0, 1},
  {1, 1, 1},
  {0, 0, 1},
  {0, 0, 1}
}, {
  {1, 1, 1},
  {1, 0, 0},
  {1, 1, 1},
  {0, 0, 1},
  {1, 1, 1}
}, {
  {1, 1, 1},
  {1, 0, 0},
  {1, 1, 1},
  {1, 0, 1},
  {1, 1, 1}
}, {
  {1, 1, 1},
  {0, 0, 1},
  {0, 0, 1},
  {0, 0, 1},
  {0, 0, 1}
}, {
  {1, 1, 1},
  {1, 0, 1},
  {1, 1, 1},
  {1, 0, 1},
  {1, 1, 1}
}, {
  {1, 1, 1},
  {1, 0, 1},
  {1, 1, 1},
  {0, 0, 1},
  {1, 1, 1}
}};

OPTIONAL static const uint8_t font_thin[][7][5] = {{
  {1, 1, 1, 1, 1},
  {1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1},
  {1, 1, 1, 1, 1}
}, {
  {0, 1, 1, 0, 0},
  {0, 0, 1, 0, 0},
  {0, 0, 1, 0, 0},
  {0, 0, 1, 0, 0},
  {0, 0, 1, 0, 0},
  {0, 0, 1, 0, 0},
  {0, 1, 1, 1, 0}
}, {
  {1, 1, 1, 1, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0},
  {1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1}
}, {
  {1, 1, 1, 1, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1}
}, {
  {1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1},
  {1, 1, 1, 1, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1}
}, {
  {1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0},
  {1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1}
}, {
  {1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0},
  {1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1},
  {1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1},
  {1, 1, 1, 1, 1}
}, {
  {1, 1, 1, 1, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1}
}, {
  {1, 1, 1, 1, 1},
  {1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1},
  {1, 1, 1, 1, 1},
  {1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1},
  {1, 1, 1, 1, 1}
}, {
  {1, 1, 1, 1, 1},
  {1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1},
  {1, 1, 1, 1, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1}
}};

#define FONT_HEIGHT ((int)(sizeof *FONT / sizeof **FONT))
#define FONT_WIDTH ((int)(sizeof **FONT))

// The four digits, two by two, measured in cells.
#define BLOCK_W (2 * FONT_WIDTH + SPACING_X)
#define BLOCK_H (2 * FONT_HEIGHT + SPACING_Y)

// No real display gets anywhere near this; it just bounds the stripe table.
#define MAX_TILE 32

static const int DIGIT_X[] = {0, FONT_WIDTH + SPACING_X};
static const int DIGIT_Y[] = {0, FONT_HEIGHT + SPACING_Y};

static Window *s_window;
static Layer *s_display_layer;

// Which cells of the digit block are lit, and where that block sits onscreen.
static uint8_t s_cells[BLOCK_H][BLOCK_W];
static int s_tile;
static int s_origin_x;
static int s_origin_y;
// The stripe profile resampled to the current cell size.
static uint8_t s_ink[MAX_TILE];

static bool block_fits(int tile, GRect bounds) {
  int w = BLOCK_W * tile;
  int h = BLOCK_H * tile;
#if defined(PBL_ROUND)
  // On a round screen the corners are missing, so what has to fit inside the
  // display is the block's diagonal rather than its width and height.
  int d = bounds.size.w < bounds.size.h ? bounds.size.w : bounds.size.h;
  return w * w + h * h <= d * d;
#else
  return w <= bounds.size.w && h <= bounds.size.h;
#endif
}

static void compute_layout(GRect bounds) {
  int tile = 1;
  while (tile < MAX_TILE && block_fits(tile + 1, bounds)) {
    tile++;
  }
  s_tile = tile;
  s_origin_x = bounds.origin.x + (bounds.size.w - BLOCK_W * tile) / 2;
  s_origin_y = bounds.origin.y + (bounds.size.h - BLOCK_H * tile) / 2;

  // Scale the stripes with the digits so the illusion reads the same on every
  // screen size.
  for (int i = 0; i < tile; i++) {
    s_ink[i] = LINE[i * LINE_N / tile];
  }
}

static unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }
  unsigned short display_hour = hour % 12;
  return display_hour ? display_hour : 12;
}

static void set_digit(unsigned short d, int col, int row) {
  for (int j = 0; j < FONT_HEIGHT; j++) {
    for (int i = 0; i < FONT_WIDTH; i++) {
      s_cells[DIGIT_Y[row] + j][DIGIT_X[col] + i] = FONT[d][j][i];
    }
  }
}

static void set_time(struct tm *t) {
  unsigned short top = DEBUG ? t->tm_min : get_display_hour(t->tm_hour);
  unsigned short bottom = DEBUG ? t->tm_sec : t->tm_min;

  memset(s_cells, 0, sizeof s_cells);
  set_digit(top / 10, 0, 0);
  set_digit(top % 10, 1, 0);
  set_digit(bottom / 10, 0, 1);
  set_digit(bottom % 10, 1, 1);
}

// Is this pixel inside a lit cell of one of the digits?
static bool in_digit(int x, int y) {
  int dx = x - s_origin_x;
  int dy = y - s_origin_y;
  if (dx < 0 || dy < 0) {
    return false;
  }
  int cx = dx / s_tile;
  int cy = dy / s_tile;
  if (cx >= BLOCK_W || cy >= BLOCK_H) {
    return false;
  }
  return s_cells[cy][cx];
}

// Stripes run up-right through the digits and down-right across the
// background; only where the two meet is there an edge to see.
static bool is_ink(int x, int y, bool fg) {
  int dx = x - s_origin_x;
  int dy = y - s_origin_y;
  int phase = (fg ? dx + dy : dx - dy) % s_tile;
  if (phase < 0) {
    phase += s_tile;
  }
  return s_ink[phase];
}

static void set_pixel(GBitmapDataRowInfo info, int x, bool ink) {
#if defined(PBL_COLOR)
  GColor color = ink ? GColorBlack : GColorWhite;
  info.data[x] = color.argb;
#else
  // One bit per pixel, and a set bit is white.
  uint8_t mask = 1 << (x % 8);
  if (ink) {
    info.data[x / 8] &= ~mask;
  } else {
    info.data[x / 8] |= mask;
  }
#endif
}

static void display_layer_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  set_time(localtime(&now));

  // Centre the digits on whatever the watch is actually showing us, so they
  // stay put when something like the timeline peek covers the bottom.
  compute_layout(layer_get_unobstructed_bounds(layer));

  // Painting the framebuffer directly is far cheaper than a draw call per
  // pixel, which matters when the screen is 260x260.
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  GRect bounds = gbitmap_get_bounds(fb);
  for (int y = bounds.origin.y; y < bounds.origin.y + bounds.size.h; y++) {
    GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y);
    for (int x = info.min_x; x <= info.max_x; x++) {
      set_pixel(info, x, is_ink(x, y, in_digit(x, y)));
    }
  }
  graphics_release_frame_buffer(ctx, fb);
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_display_layer);
}

// On platforms without a timeline peek the subscribe below compiles away,
// and with it any reference to this handler.
OPTIONAL static void handle_unobstructed_change(void *context) {
  layer_mark_dirty(s_display_layer);
}

static void init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);

  Layer *root_layer = window_get_root_layer(s_window);
  s_display_layer = layer_create(layer_get_bounds(root_layer));
  layer_set_update_proc(s_display_layer, display_layer_update_proc);
  layer_add_child(root_layer, s_display_layer);

  window_stack_push(s_window, true /* animated */);

  tick_timer_service_subscribe(DEBUG ? SECOND_UNIT : MINUTE_UNIT, handle_tick);
  unobstructed_area_service_subscribe(
      (UnobstructedAreaHandlers){.did_change = handle_unobstructed_change},
      NULL);
}

static void deinit(void) {
  unobstructed_area_service_unsubscribe();
  tick_timer_service_unsubscribe();
  layer_destroy(s_display_layer);
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
