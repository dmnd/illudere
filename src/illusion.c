#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { \
  0x97, 0x43, 0x2C, 0xDD, 0x14, 0xAC, 0x4E, 0x45, \
  0x9F, 0xBC, 0x20, 0xF1, 0xAF, 0x30, 0xC1, 0xE9 \
}

PBL_APP_INFO(MY_UUID,
             "Illusion", "Desmond Brand",
             1, 1, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define DEBUG false
#define TILE_SIZE 10
const unsigned char LINE[] = {0, 0, 1, 1, 1, 1, 1, 0, 0, 0};
#define FONT font_thin

// distance on left and right of text
#define PADDING_X 3
#define PADDING_Y 1

// distance between characters
#define SPACING_X 2
#define SPACING_Y 2

size_t LINE_N = sizeof LINE / sizeof *LINE;

unsigned char font_thick[][5][3] =  {{
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

unsigned char font_thin[][7][4] =  {{
  {1, 1, 1, 1},
  {1, 0, 0, 1},
  {1, 0, 0, 1},
  {1, 0, 0, 1},
  {1, 0, 0, 1},
  {1, 0, 0, 1},
  {1, 1, 1, 1}
}, {
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 0},
  {0, 1, 1, 1}
}, {
  {1, 1, 1, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1},
  {1, 1, 1, 1},
  {1, 0, 0, 0},
  {1, 0, 0, 0},
  {1, 1, 1, 1}
}, {
  {1, 1, 1, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1},
  {1, 1, 1, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1},
  {1, 1, 1, 1}
}, {
  {1, 0, 0, 1},
  {1, 0, 0, 1},
  {1, 0, 0, 1},
  {1, 1, 1, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1}
}, {
  {1, 1, 1, 1},
  {1, 0, 0, 0},
  {1, 0, 0, 0},
  {1, 1, 1, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1},
  {1, 1, 1, 1}
}, {
  {1, 1, 1, 1},
  {1, 0, 0, 0},
  {1, 0, 0, 0},
  {1, 1, 1, 1},
  {1, 0, 0, 1},
  {1, 0, 0, 1},
  {1, 1, 1, 1}
}, {
  {1, 1, 1, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1}
}, {
  {1, 1, 1, 1},
  {1, 0, 0, 1},
  {1, 0, 0, 1},
  {1, 1, 1, 1},
  {1, 0, 0, 1},
  {1, 0, 0, 1},
  {1, 1, 1, 1}
}, {
  {1, 1, 1, 1},
  {1, 0, 0, 1},
  {1, 0, 0, 1},
  {1, 1, 1, 1},
  {0, 0, 0, 1},
  {0, 0, 0, 1},
  {1, 1, 1, 1}
}};

#define FONT_HEIGHT (sizeof *FONT / sizeof **FONT)
#define FONT_WIDTH (sizeof **FONT)

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168

#define TILES_X ( \
  PADDING_X + FONT_WIDTH + SPACING_X + FONT_WIDTH + PADDING_X)
#define TILES_Y ( \
  PADDING_Y + FONT_HEIGHT + SPACING_Y + FONT_HEIGHT + PADDING_Y)

#define ORIGIN_X ((SCREEN_WIDTH - TILES_X*TILE_SIZE)/2)
#define ORIGIN_Y ((SCREEN_HEIGHT - TILES_Y*TILE_SIZE)/2)

unsigned char frames[2][TILES_X][TILES_Y];
size_t current_frame = 0;
size_t prev_frame = 1;

unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }
  unsigned short display_hour = hour % 12;
  return display_hour ? display_hour : 12;
}

#define OFFSET 0
unsigned char get_pixel(size_t i, size_t j, bool fg) {
  int fgf = fg ? 1 : -1;
  size_t x = ((i + j*fgf + TILE_SIZE + OFFSET)%TILE_SIZE) * LINE_N/TILE_SIZE;
  return LINE[x % LINE_N];
};

// TODO: Do the work to generate the bitmap only once, then use
// graphics_draw_bitmap_in_rect? It's probably faster.
#define REVERSE_STROKE false
void draw_tile(GContext* ctx, size_t x, size_t y, bool fg) {
  for (size_t j = 0; j < TILE_SIZE; j++) {
    for (size_t i = 0; i < TILE_SIZE; i++) {
      if (get_pixel(i, j, fg)) {
        graphics_context_set_stroke_color(ctx, GColorBlack);
      } else {
        graphics_context_set_stroke_color(ctx, GColorWhite);
      }

      graphics_draw_pixel(ctx, GPoint(
        ORIGIN_X + x*TILE_SIZE + i,
        ORIGIN_Y + y*TILE_SIZE + j));
    }
  }
};

size_t DIGIT_X[] = {PADDING_X, PADDING_X + FONT_WIDTH+SPACING_X};
size_t DIGIT_Y[] = {PADDING_Y, PADDING_Y + FONT_HEIGHT+SPACING_Y};

void draw_digit(unsigned short d, size_t x, size_t y) {
  for (size_t j = 0; j < FONT_HEIGHT; j++) {
    for (size_t i = 0; i < FONT_WIDTH; i++) {
      frames[current_frame][DIGIT_X[x]+i][DIGIT_Y[y]+j] = FONT[d][j][i];
    }
  }
}

void draw_rect(size_t x1, size_t y1, size_t x2, size_t y2, unsigned char v) {
  for (size_t y = y1; y < y2; y++) {
    for (size_t x = 0; x < x2; x++) {
      frames[current_frame][x][y] = v;
    }
  }
}

void swap(size_t* a, size_t* b) {
  size_t temp = *a;
  *a = *b;
  *b = temp;
}

void display_layer_update_cb(Layer *me, GContext* ctx) {
  PblTm t;
  get_time(&t);
  unsigned short display_hour = get_display_hour(t.tm_hour);
  unsigned short display_min = t.tm_min;
  unsigned short display_sec = t.tm_sec;

  unsigned short row1, row2;
  if (DEBUG) {
    row1 = display_min;
    row2 = display_sec;
  } else {
    row1 = display_hour;
    row2 = display_min;
  }

  draw_rect(0, 0, TILES_X, TILES_Y, 0);
  draw_digit(row1/10, 0, 0);
  draw_digit(row1%10, 1, 0);
  draw_digit(row2/10, 0, 1);
  draw_digit(row2%10, 1, 1);

  // now redraw tiles that have changed
  for (size_t j = 0; j < TILES_Y; j++) {
    for (size_t i = 0; i < TILES_X; i++) {
      unsigned char v = frames[current_frame][i][j];
      // TODO: make this only redraw when tiles have changed. At the moment,
      // I am marking the entire display_layer as dirty which seems to mean
      // I have to repaint the whole thing. Probably what I want is is separate
      // layer for each tile?

      // if (v != frames[prev_frame][i][j]) {
        draw_tile(ctx, i, j, v == 1);
        frames[prev_frame][i][j] = v;
      // }
    }
  }

  swap(&current_frame, &prev_frame);
}

Window window;
Layer display_layer;

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  layer_mark_dirty(&display_layer);
}

void handle_init(AppContextRef ctx) {
  window_init(&window, "Illusion");
  window_stack_push(&window, true /* Animated */);

  // init both frames to 2
  draw_rect(0, 0, TILES_X, TILES_Y, 2);
  draw_rect(0, 0, TILES_X, TILES_Y, 2);

  // Init the layer for the display
  layer_init(&display_layer, window.layer.frame);
  display_layer.update_proc = &display_layer_update_cb;
  layer_add_child(&window.layer, &display_layer);
}

void pbl_main(void *params) {
  unsigned char units;
  if (DEBUG) {
    units = SECOND_UNIT;
  } else {
    units = MINUTE_UNIT;
  }
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = units
    }
  };
  app_event_loop(params, &handlers);
}
