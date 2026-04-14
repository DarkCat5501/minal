#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BELL        '\a'
#define DEL         0x7f
#define BACKSPACE   '\b' 
#define FORMFEED    '\f'
#define TAB         '\t'
#define LINEFEED    '\n'
#define CARRIAGERET '\r'
#define ESC         '\033'

typedef struct {
  struct { uint8_t r,g,b; } fg_color;
  struct { uint8_t r,g,b; } bg_color;
  struct { uint8_t r,g,b; } ul_color;

  struct {
    uint64_t hide_cursor:1;
    uint64_t hide_scrollbar:1;
    uint64_t stop_cursorblink:1;
    uint64_t stop_cursorblink2:1;
  } flags;

} Escape_Cmd;


void ansi_debug(const char* data, size_t len);
bool ansi_find_cmd_end(const char* data, size_t len, size_t* out_len);
int ansi_str_to_int(const char* str, size_t len, int def);
bool ansi_split_args(const char* data, size_t len, const char** out, size_t* out_len);
bool ansi_split_int_args(const char* data, size_t len, int* out_n, int def);
