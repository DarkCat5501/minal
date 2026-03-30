#ifndef MINAL_H_
#define MINAL_H_

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utmp.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_ttf/SDL_ttf.h>

#define Kib 1024
#define Mib (Kib * Kib)
#define Gib (Kib * Kib * Kib)

#define Kb 1000
#define Mb (Kb * Kb)
#define Gb (Kb * Kb * Kb)

#define _32K 32 * Kb

#define CARR_SB_INIT_CAP _32K
#define CARR_SV_IMPLEMENTATION
#include "carrlib/sv.h"

#include "carrlib/vec.h"

#define FONT_FILE "resources/font.ttf"
#define DEFAULT_FONT_SIZE 10.0f
#define DEFAULT_DISPLAY_DPI 96
#define DEFAULT_N_COLS 80
#define DEFAULT_N_ROWS 24

#define FPS 60

typedef struct
{
  TTF_Font* font;
  float font_size;
  int display_dpi;
  int cell_height;
  int cell_width;
} Config;

typedef struct
{
  size_t row, col;
} Cursor;

#define ArrayMeta                                                              \
  struct                                                                       \
  {                                                                            \
    size_t len, cap;                                                           \
  }

typedef struct
{
  uint8_t* items;
  size_t utf_len;
  ArrayMeta;
} Line;

typedef union
{
  SDL_Color rgba;
  uint32_t hex;
} Color;

typedef union
{
  struct
  {
    Color bg,fg;
    Color c[16];
  };
  Color colors[34];
} ColorPalette;

typedef struct
{
  // SDL_FRect position;
  // SDL_FRect margin;
  // SDL_FRect padding;
  // SDL_FRect border;
  // Color border_color;

  ColorPalette palett;
  // int z_index;       // TODO: implement z-index

  // bool float: 1;
  // bool relative_position : 1;
  // bool no_scroll : 1;
  // bool no_wrap : 1;
} BufferConfig;

typedef struct
{
  BufferConfig config;

  // calculated
  SDL_FPoint  _scroll;
  int      _scroll_line;
  SDL_FRect   _rect;
  Cursor      _cursor_max;
  bool visible: 1;
  // data
  Line*       items;
  ArrayMeta;
} MinalBuffer;
typedef struct
{
  MinalBuffer* items;
  ArrayMeta;
} MinalBufferList;

typedef struct
{
  MinalBufferList buffers;
  size_t buffer_idx;
  Cursor cursor;

  Config config;
  bool run;

  SDL_Window* window;
  SDL_Renderer* rend;
  SDL_Rect window_rect;

  TTF_TextEngine* text_engine;
  TTF_Text* text;

  size_t input_start;
  pid_t shell_pid;
  int slave_fd;
  int master_fd;

  bool bracket_mode;
} Minal;

// basic stuff
Minal minal_init();
void minal_spawn_shell(Minal* m);
void minal_finish(Minal* m);
void minal_run(Minal* m);

// ansi escape sequences commands
void minal_parse_ansi(Minal* m, StringView* bytes);
void minal_erase_in_line(Minal* m, size_t opt);
void minal_erase_in_display(Minal* m, size_t opt);

// cursor
SDL_FRect minal_cursor_to_rect(Minal* m);
void minal_cursor_move(Minal* m, int new_col, int new_row);

// write to subprocess's stdin
void minal_write_str(Minal* m, const char* s);
void minal_write_char(Minal* m, int c);
void minal_transmitter(Minal* m);

// read from subprocess's stdout and render it
int minal_read_nonblock(Minal* m, char* buf, size_t n);
void minal_receiver(Minal* m);

// read/write
uint8_t minal_at(Minal* m, size_t col, size_t row);
void minal_insert_at(Minal* m, size_t col, size_t row, uint8_t* c);

// line
void line_grow(Line* l);
size_t line_col2idx(Line* l, size_t col);
void line_printf(Line* l);

// helpers
const char* SDLK_to_ansicode(SDL_Keycode key);

// ASCII CODES (C0 control codes)
#define BELL 0x07
#define BACKSPACE 0x08
#define TAB 0x09
#define LINEFEED 0x0A
#define VERTTAB 0x0B
#define FORMFEED 0x0C
#define CARRIAGERET 0x0D
#define ESC 0x1B
#define DEL 0x7F

// C1 control codes                 ESC+
#define SINGLE_SHIFT_TWO "\x8E"
#define SINGLE_SHIFT_THREE "\x8F"
#define DEVICE_CONTROL_STRING "\x90"
#define CONTROL_SEQUENCE_INTRODUCER '['
#define STRING_TERMINATOR "\x9C"
#define OPERATING_SYSTEM_COMMAND "\x9D"
#define START_OF_STRING "\x98"
#define PRIVACY_MESSAGE "\x9E"
#define APPLICATION_PROGRAM_COMMAND "\x9F"

// CSI commands                     Code       Usage
#define CURSOR_UP 'A'                    // CSI n A
#define CURSOR_DOWN 'B'                  // CSI n B
#define CURSOR_FORWARD 'C'               // CSI n C
#define CURSOR_BACK 'D'                  // CSI n D
#define CURSOR_NEXT_LINE 'E'             // CSI n E
#define CURSOR_PREVIOUS_LINE 'F'         // CSI n F
#define CURSOR_HORIZONTAL_ABSOLUTE 'G'   // CSI n G
#define CURSOR_POSITION 'H'              // CSI n ; m H
#define ERASE_IN_DISPLAY 'J'             // CSI n J
#define ERASE_IN_LINE 'K'                // CSI n K
#define SCROLL_UP 'S'                    // CSI n S
#define SCROLL_DOWN 'T'                  // CSI n T
#define HORIZONTAL_VERTICAL_POSITION 'f' // CSI n ; m f
#define SELECT_GRAPHIC_RENDITION 'm'     // CSI n m
#define AUX_PORT_ON "5i"                 // CSI 5i
#define AUX_PORT_OFF "4i"                // CSI 4i
#define DEVICE_STATUS_REPORT "6n"        // CSI 6n

// ANSI ESCAPE CODES
// #define MOVE_UP    ESC"[A"
// #define MOVE_DOWN  ESC"[B"
// #define MOVE_RIGHT ESC"[C"
// #define MOVE_LEFT  ESC"[D"
// #define MOVE_LEFT  ESC"[K"

#endif // MINAL_H_
