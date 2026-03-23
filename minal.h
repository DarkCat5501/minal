#ifndef MINAL_H_
#define MINAL_H_

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmp.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_ttf/SDL_ttf.h>

#define _32K 32678

#define CARR_SB_INIT_CAP _32K
#define CARR_SV_IMPLEMENTATION
#include "carrlib/sv.h"

#include "carrlib/vec.h"

#define FONT_FILE "resources/font.ttf"
#define DEFAULT_FONT_SIZE   18.0f
#define DEFAULT_DISPLAY_DPI 96
#define DEFAULT_N_COLS      80
#define DEFAULT_N_ROWS      24

#define FPS    60

// ASCII CODES
#define BELL      0x07
#define BACKSPACE 0x08
#define LINEFEED  0X0A
#define DEL       0x7F

// ANSI ESCAPE CODES
#define ESC        "\x1B"
#define MOVE_UP    ESC"[1A"
#define MOVE_DOWN  ESC"[1B"
#define MOVE_RIGHT ESC"[1C"
#define MOVE_LEFT  ESC"[1D"

typedef struct {
    TTF_Font* font;
    float     font_size;
    int       display_dpi;
    int       n_cols;
    int       n_rows;
    int       cell_height;
    int       cell_width;
    int       window_width;
    int       window_height;
} Config;

typedef struct {
    size_t row;
    size_t col;
} Cursor;

typedef struct {
    uint8_t* items;
    size_t   len;
    size_t   cap;
} Line;

typedef struct {
    Line*  items;
    size_t len;
    size_t cap;
} Display;

typedef struct {
    Display         display;
    StringBuilder   display_str;
    Cursor          cursor;
    Config          config;
    bool            run;

    SDL_Color       fg_color;
    SDL_Color       bg_color;
    SDL_Window*     window;
    SDL_Renderer*   rend;

    TTF_TextEngine* text_engine;
    TTF_Text*       text;

    size_t          input_start;
    int             slave_fd;
    int             master_fd;
} Minal;

// basic stuff
Minal       minal_init();
void        minal_spawn_shell(Minal* m);
void        minal_finish(Minal *m);
void        minal_main(Minal* m);

// cursor 
SDL_FRect   minal_cursor_to_rect(Minal* m);
void        minal_cursor_move(Minal* m, int new_col, int new_row);

// write to subprocess's stdin
void        minal_write_str(Minal* m, const char* s);
void        minal_write_char(Minal* m, int c);
void        minal_transmitter(Minal* m, SDL_Event* event);

// read from subprocess's stdout and render it
int         minal_read_nonblock(Minal* m, char* buf);
void        minal_receiver(Minal* m);

// helpers
size_t      minal_line_col2idx(Minal* m, size_t col, size_t row);
size_t      minal_line_chars(Minal* m, size_t row);
size_t      minal_row_size(Minal* m, size_t row);
uint8_t     minal_at(Minal *m, size_t col, size_t row);
void        minal_insert_at(Minal* m, size_t col, size_t row, char c);
const char* SDLK_to_ansicode(SDL_Keycode key);
bool        is_utf8_head(uint8_t ch);
size_t      utf8_chrlen(char ch);

#endif // MINAL_H_
