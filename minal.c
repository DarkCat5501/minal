#include "minal.h"

bool is_utf8_head(uint8_t ch) {
  return ((ch >> 7) & 1) == 1 && (ch >> 6) != 0b10;
}

size_t utf8_chrlen(char ch) {
  uint8_t b = (uint8_t)ch;
  size_t size = 0;
  for (size_t i = 0; i < 4; i++) {
    if ((b >> (7 - i) & 1) == 1) {
      size += 1;
    } else {
      break;
    }
  }
  return size == 0 ? 1 : size;
}

extern char **environ;

void minal_spawn_shell(Minal *m) {
  int master_fd;
  int slave_fd;
  if (openpty(&master_fd, &slave_fd, NULL, NULL, NULL) == -1) {
    printf("ERROR: openpty failed: %s\n", strerror(errno));
    exit(1);
  };

  m->shell_pid = fork();
  if (m->shell_pid == 0) {
    login_tty(slave_fd);
    char *defaultshell = getenv("SHELL");
    char cols[50];
    char rows[50];
    char *path = "/usr/bin/bash";
    setenv("SHELL", path, 1);
    // if (defaultshell != NULL)
    //   path = defaultshell;

    sprintf(cols, "%d", m->config.n_cols);
    sprintf(rows, "%d", m->config.n_rows);
    setenv("COLUMNS", cols, 1);
    setenv("LINES", rows, 1);

    char *argv[] = {path, NULL};
    if (execve(path, argv, environ) == -1) {
      printf("ERROR: execv: %s\n", strerror(errno));
      exit(1);
    };
    usleep(2000);
    exit(0);
  }
  m->master_fd = master_fd;
  m->slave_fd = slave_fd;
}

void minal_check_shell(Minal *m) {
  int status;
  pid_t result = waitpid(m->shell_pid, &status, WNOHANG);
  if (result == m->shell_pid) {
    if (WIFEXITED(status)) {
      printf("Shell exited with code %d\n", WEXITSTATUS(status));
      m->run = false;
    } else if (WIFSIGNALED(status)) {
      printf("Shell signaled with %d\n", WTERMSIG(status));
      m->run = false;
    }
  }
}

// TODO: create a minal resize display
void minal_free_display(Minal *m) {
  for (size_t i = 0; i < m->display.len; ++i)
    vec_free(&m->display.items[i]);
  vec_free(&m->display);
  sb_free(&m->display_str);
}

void minal_resize_font(Minal *m) {
  Config *c = &m->config;
  SDL_GetWindowSize(m->window, &c->window_width, &c->window_height);
  assert(TTF_SetFontSizeDPI(c->font, c->font_size, c->display_dpi,
                            c->display_dpi));
  assert(TTF_GetStringSize(c->font, " ", 1, &c->cell_width, &c->cell_height));
  c->n_cols = c->window_width / c->cell_width;
  c->n_rows = c->window_height / c->cell_height;
}

void minal_resize_display(Minal *m) {
  static int started = 0;

  if (!started++) {
    Display *d = &m->display;
    vec_expandto(d, m->config.n_rows);
    for (int i = 0; i < m->config.n_rows; ++i) {
      Line l = {0};
      vec_expandto(&l, m->config.n_cols);
      memset(l.items, 0, l.cap * sizeof(char));
      vec_append(d, l);
    }
    m->display_str = sb_with_cap(4 * m->config.n_rows * m->config.n_cols);
  }
}

Minal minal_init() {
  Minal m = {0};

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  assert(TTF_Init());

  // default configs
  m.config.font_size = DEFAULT_FONT_SIZE;
  m.config.display_dpi = DEFAULT_DISPLAY_DPI;

  minal_spawn_shell(&m);
  m.run = true;

  m.config.font = TTF_OpenFont(FONT_FILE, m.config.font_size);
  assert(m.config.font != NULL && "Could not initialize font");
  m.window = SDL_CreateWindow("Minal", m.config.window_width,
                              m.config.window_height, SDL_WINDOW_RESIZABLE);
  assert(m.window != NULL && "Could not initialize window");
  m.rend = SDL_CreateRenderer(m.window, NULL);
  SDL_SetRenderVSync(m.rend, 1);
  assert(m.rend != NULL && "Could not initialize renderer");
  m.text_engine = TTF_CreateRendererTextEngine(m.rend);
  assert(m.text_engine != NULL && "Could not initialize text engine");
  m.text = TTF_CreateText(m.text_engine, m.config.font, "", 0);
  assert(m.text != NULL && "Could not initialize text object");
  m.fg_color = (SDL_Color){0xfd, 0xfd, 0xfd, 0xff};
  m.bg_color = (SDL_Color){0x1a, 0x1a, 0x1a, 0xff};

  minal_resize_font(&m);
  minal_resize_display(&m);
  return m;
}

void minal_parse_ansi(Minal *m, StringView *bytes) {
  uint8_t b = sv_chop_left(bytes);

  if (b == CONTROL_SEQUENCE_INTRODUCER) {
    b = sv_first(bytes);

    if (b == '?') {
      sv_chop_left(bytes);

      int opt = -1;
      if (isdigit(sv_first(bytes))) {
        opt = sv_parse_int(bytes);
      }

      b = sv_chop_left(bytes);

      if (opt == 25) {
      }

      if (opt == 1004) {
      }

      if (opt == 1049) {
      }

      if (opt == 2004) {
        if (b == 'h') {
          m->bracket_mode = true;
        }

        if (b == 'l') {
          m->bracket_mode = false;
        }
      }

      return;
    }

    int argv[10] = {-1};
    int argc = 0;

    while (isdigit(sv_first(bytes))) {
      argv[argc++] = sv_parse_int(bytes);
      if (sv_first(bytes) != ';') {
        break;
      };
      sv_chop_left(bytes);
    }

    b = sv_chop_left(bytes);
    switch (b) {

    case CURSOR_UP: {
      int opt = argc > 0 ? argv[0] : 1;

      size_t new_row;
      if (m->cursor.row < opt) {
        new_row = 0;
      } else {
        new_row = m->cursor.row - opt;
      }
      minal_cursor_move(m, m->cursor.col, new_row);
    }; break;

    case CURSOR_DOWN: {
      int opt = argc > 0 ? argv[0] : 1;

      size_t new_row;
      if (m->cursor.row + opt >= m->config.n_rows) {
        new_row = m->config.n_rows - 1;
      } else {
        new_row = m->cursor.row + opt;
      }
      minal_cursor_move(m, m->cursor.col, new_row);
    }; break;

    case CURSOR_FORWARD: {
      int opt = argc > 0 ? argv[0] : 1;

      size_t new_col;
      if (m->cursor.col + opt >= m->config.n_cols) {
        new_col = m->config.n_cols - 1;
      } else {
        new_col = m->cursor.col + opt;
      }
      minal_cursor_move(m, new_col, m->cursor.row);
    }; break;

    case CURSOR_BACK: {
      int opt = argc > 0 ? argv[0] : 1;

      size_t new_col;
      if (m->cursor.col < opt) {
        new_col = 0;
      } else {
        new_col = m->cursor.col - opt;
      }
      minal_cursor_move(m, new_col, m->cursor.row);
    }; break;

    case CURSOR_POSITION: {
      int opt1 = argc > 0 ? argv[0] : 1;
      int opt2 = argc > 1 ? argv[1] : 1;

      size_t new_col = MIN(MAX(0, opt1 - 1), m->config.n_cols - 1);
      size_t new_row = MIN(MAX(0, opt2 - 1), m->config.n_rows - 1);
      minal_cursor_move(m, new_col, new_row);
    }; break;

    case ERASE_IN_DISPLAY: {
      size_t opt;
      if (argv[0] == -1) {
        opt = 0;
      }
      minal_erase_in_display(m, opt);
    }; break;

    case ERASE_IN_LINE: {
      size_t opt;
      if (argv[0] == -1) {
        opt = 0;
      }
      minal_erase_in_line(m, opt);
    }; break;
    }
  }
}

SDL_FRect minal_cursor_to_rect(Minal *m) {
  return (SDL_FRect){
      .x = m->cursor.col * m->config.cell_width,
      .y = m->cursor.row * m->config.cell_height,
      .w = m->config.cell_width,
      .h = m->config.cell_height,
  };
}

void minal_cursor_move(Minal *m, int new_col, int new_row) {
  m->cursor.col = new_col;
  m->cursor.row = new_row;
}

void minal_write_str(Minal *m, const char *s) {
  size_t n = strlen(s);
  write(m->master_fd, s, n);
}

void minal_write_char(Minal *m, int c) { write(m->master_fd, &c, 1); }

int minal_read_nonblock(Minal *m, char *buf, size_t n) {
  int oldf = fcntl(m->master_fd, F_GETFL, 0);
  assert(oldf != -1);
  int ret = fcntl(m->master_fd, F_SETFL, oldf | O_NONBLOCK);
  assert(ret != -1);
  int res = read(m->master_fd, buf, n);
  if (res == -1) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      printf("ERROR: read nonblock: %s\n", strerror(errno));
      exit(1);
    }
    return 0;
  }
  return res;
}

size_t line_col2idx(Line *l, size_t col)
/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
// col:  0  1   2            3  4   5      6  7   8         9 //
//     [ _, _, {_, _, _, _}, _, _, {_, _}, _, _, {_, _, _}, _ ] //
// idx:  0  1   2  3  4  5   6  7   8  9  10 11  12 13 14  15 //
//              ^ //
//           start = 2 // start + size = start + 4 = 2 + 4 = 6 // start + size -
//           size = start                                       //
// col = 2  => col2idx = 2 // col = 3  => col2idx = 6 // col = 5  => col2idx = 8
// // col = 8  => col2idx = 12 // col = 10 => col2idx = 16 // col = 11 =>
// col2idx = 17                                                    // col = 15
// => col2idx = 21                                                    //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////
{
  if (l->len == 0)
    return 0;

  size_t i = 0;
  size_t cols = 0;
  while (cols < col) {
    if (i >= l->len) {
      i++;
    } else {
      i += utf8_chrlen(l->items[i]);
    }
    cols++;
  }

  return i;
}

void line_grow(Line *l) {
  vec_grow(l);
  return;
}

void line_print(Line *line) {
  printf("    Line After: {\n");
  printf("     cap:   %zu,\n", line->cap);
  printf("     len:   %zu,\n", line->len);
  printf("     items: [ \n");
  for (size_t j = 0; j < line->len; ++j) {
    uint8_t it = line->items[j];
    printf("        - %08b (%c)\n", it, it);
  }
  printf("   ]}\n");
}

uint8_t minal_at(Minal *m, size_t col, size_t row) {
  assert(row >= 0 && row < m->display.len);

  size_t idx = line_col2idx(&m->display.items[row], col);
  assert(idx >= 0 && idx < m->display.items[row].len);

  return m->display.items[row].items[idx];
}

void minal_append(Minal *m, size_t row, char c) {
  assert(row >= 0 && row < m->display.len);
  Line r = m->display.items[row];
  vec_append(&m->display.items[row], (uint8_t)c);
}

void minal_insert_at(Minal *m, size_t col, size_t row, uint8_t *c) {
  assert(row >= 0 && row < m->display.len && "TODO: handle create new lines");

  Line *l = &m->display.items[row];
  size_t idx = line_col2idx(l, col);
  size_t new_size = utf8_chrlen(*c);

  if (idx >= l->len) {
    int n = idx - l->len;
    for (int i = 0; i < n; ++i) {
      minal_append(m, row, ' ');
    }
    for (size_t i = 0; i < new_size; ++i) {
      minal_append(m, row, *(c + i));
    }
    return;
  }

  uint8_t head = l->items[idx];
  size_t tail = line_col2idx(l, col + 1);
  size_t old_size = utf8_chrlen(head);
  int diff = (new_size - old_size);

  if (diff != 0) {
    if (l->len + diff >= l->cap) {
      line_grow(l);
    }

    if (l->len > tail) {
      size_t n = sizeof(uint8_t) * (l->len - tail);
      void *src = &l->items[tail];
      void *dst = &l->items[tail + diff];
      memmove(dst, src, n);

      int absdiff = abs(diff);
      size_t udiff = (size_t)absdiff;
      if (diff < 0) {
        l->len -= udiff;
      } else {
        l->len += udiff;
      }
    }
  }

  for (size_t i = 0; i < new_size; ++i) {
    uint8_t byte = *(c + i);
    l->items[idx + i] = byte;
  }
}

void minal_erase_in_line(Minal *m, size_t opt)
//////////////////////////////////////////////
//  opt=0                                   //
//               start             end      //
//               v                 v        //
//  [_, _, _, _, X, _, _, _, _, _, _]       //
//                                          //
//  opt=1                                   //
//   start       end                        //
//   v           v                          //
//  [_, _, _, _, X, _, _, _, _, _, _]       //
//                                          //
//  opt=2                                   //
//   start                         end      //
//   v                             v        //
//  [_, _, _, _, X, _, _, _, _, _, _]       //
//                                          //
//////////////////////////////////////////////
{
  size_t x = m->cursor.col;
  size_t y = m->cursor.row;
  Line *line = &m->display.items[y];
  size_t start;
  size_t end;

  if (opt == 1 || opt == 2) {
    start = 0;
  } else {
    start = line_col2idx(&m->display.items[y], x);
  }

  if (opt == 0 || opt == 2) {
    end = line->len - 1;
  } else {
    end = line_col2idx(&m->display.items[y], x);
  }

  memset(line->items + start, ' ', end - start + 1);
}

void minal_erase_in_display(Minal *m, size_t opt) {
  size_t x = m->cursor.col;
  size_t y = m->cursor.row;

  size_t start = opt == 1 || opt == 2 ? 0 : y;
  size_t end = opt == 0 || opt == 2 ? m->config.n_rows - 1 : y;

  if (start > end) {
    size_t tmp = start;
    start = end;
    end = tmp;
  }

  for (size_t row = start; row <= end; ++row) {
    if (row == y) {
      minal_cursor_move(m, x, row);
      minal_erase_in_line(m, opt);
    } else {
      minal_cursor_move(m, 0, row);
      minal_erase_in_line(m, 2);
    }
  }

  if (opt == 2)
    minal_cursor_move(m, 0, 0);
  else
    minal_cursor_move(m, x, y);
}

void minal_receiver(Minal *m) {
  SDL_SetRenderDrawColor(m->rend, m->bg_color.r, m->bg_color.g, m->bg_color.b,
                         m->bg_color.a);
  SDL_RenderClear(m->rend);

  size_t offset = 0;
  char _buf[_32K];
  while (true) {
    assert(offset < _32K);
    int n = minal_read_nonblock(m, _buf + offset, _32K - offset);
    if (n == 0)
      break;
    offset += n;
  }

  StringView view = (StringView){
      .data = _buf,
      .len = offset,
  };

  while (view.len > 0) {
    uint8_t ch = (uint8_t)sv_chop_left(&view);

    size_t x = m->cursor.col;
    size_t y = m->cursor.row;
    size_t idx = line_col2idx(&m->display.items[y], x);

    if (ch == '\0') {
      continue;
    }

    if (ch == *BELL) {
      continue;
    }

    if (ch == *LINEFEED) {
      minal_cursor_move(m, x, y + 1);
      continue;
    }

    if (ch == *CARRIAGERET) {
      minal_cursor_move(m, 0, y);
      continue;
    }

    if (ch == *DEL) {
      printf("RECEIVED DEL\n");
    }

    if (ch == *BACKSPACE) {
      if (!(x > 0 || y > 0)) {
        continue;
      }

      if (x > 0) {
        minal_cursor_move(m, x - 1, y);
      } else {
        size_t last_col = m->config.n_cols - 1;
        minal_cursor_move(m, last_col, y - 1);
      }

      continue;
    }

    if (ch == *ESC) {
      minal_parse_ansi(m, &view);
      continue;
    }

    if (is_utf8_head(ch)) {
      size_t utf8len = utf8_chrlen(ch);
      assert(utf8len < 5);
      size_t utf8cur = 0;
      uint8_t utf8code[5];

      for (;;) {
        assert(utf8cur == 0 || (ch >> 6) == 0b10);
        utf8code[utf8cur++] = ch;
        if (utf8cur >= utf8len) {
          break;
        }
        ch = (uint8_t)sv_chop_left(&view);
      }
      utf8code[4] = '\0';

      minal_insert_at(m, x, y, (uint8_t *)utf8code);

      if (m->cursor.col == m->config.n_cols - 1) {
        minal_cursor_move(m, 0, m->cursor.row + 1);
      } else {
        minal_cursor_move(m, m->cursor.col + 1, m->cursor.row);
      }
      continue;
    }

    minal_insert_at(m, x, y, &ch);
    if (m->cursor.col == m->config.n_cols - 1) {
      minal_cursor_move(m, 0, m->cursor.row + 1);
    } else {
      minal_cursor_move(m, m->cursor.col + 1, m->cursor.row);
    }
  }

  m->display_str.len = 0;
  for (size_t row = 0; row < m->display.len; row++) {
    Line l = m->display.items[row];
    sb_nconcat(&m->display_str, (char *)l.items, l.len);
    sb_append(&m->display_str, '\n');
  }

  TTF_SetTextString(m->text, m->display_str.data, m->display_str.len);
  SDL_SetRenderDrawColor(m->rend, m->fg_color.r, m->fg_color.r, m->fg_color.r,
                         m->fg_color.a);
  TTF_DrawRendererText(m->text, 0, 0);
}

void minal_transmitter(Minal *m, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    m->run = false;
    return;
  }

  if (event->type == SDL_EVENT_WINDOW_RESIZED) {
    static int count = 0;
    if (count == 1) {
      printf("resize once\n");
      minal_resize_font(m);
      minal_resize_display(m);
    }
    count++;
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.key == SDLK_ESCAPE) {
      m->run = false;
      return;
    }
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    SDL_Keycode k = event->key.key;
    switch (k) {
    case (SDLK_F1): {
      minal_write_str(m, "𒀀");
    }; break;
    default: {
      const char *code = SDLK_to_ansicode(k);
      if (strlen(code) == 0) {
        break;
      }
      minal_write_str(m, code);
    }; break;
    }
  }
  if (event->type == SDL_EVENT_TEXT_INPUT) {
    minal_write_str(m, event->text.text);
  }
}

void minal_run(Minal *m) {
  assert(SDL_StartTextInput(m->window));
  while (m->run) {
    SDL_Event event;
    minal_check_shell(m);
    while (SDL_PollEvent(&event))
      minal_transmitter(m, &event);
    minal_receiver(m);

    SDL_SetRenderDrawColor(m->rend, m->fg_color.r, m->fg_color.g, m->fg_color.b,
                           m->fg_color.a);
    SDL_FRect cur = minal_cursor_to_rect(m);
    SDL_RenderFillRect(m->rend, &cur);
    SDL_RenderPresent(m->rend);
  }
}

void minal_finish(Minal *m) {
  for (size_t i = 0; i < m->display.len; ++i) {
    vec_free(&m->display.items[i]);
  }
  vec_free(&m->display);
  sb_free(&m->display_str);

  assert(SDL_StopTextInput(m->window));
  TTF_CloseFont(m->config.font);

  TTF_DestroyRendererTextEngine(m->text_engine);
  TTF_DestroyText(m->text);

  SDL_DestroyRenderer(m->rend);
  SDL_DestroyWindow(m->window);

  TTF_Quit();
  SDL_Quit();
}

int main(void) {
  Minal m = minal_init();
  minal_run(&m);
  minal_finish(&m);
  return 0;
}

const char *SDLK_to_ansicode(SDL_Keycode key) {
  switch (key) {
  case SDLK_BACKSPACE:
    return "\b";
  case SDLK_RETURN:
    return "\n";
  case SDLK_TAB:
    return "\t";
  case SDLK_UP:
    return "\x1B[A";
  case SDLK_DOWN:
    return "\x1B[B";
  case SDLK_RIGHT:
    return "\x1B[C";
  case SDLK_LEFT:
    return "\x1B[D";
  case SDLK_HOME:
    return "\x1B[1~";
  case SDLK_INSERT:
    return "\x1B[2~";
  case SDLK_DELETE:
    return "\x1B[3~";
  case SDLK_END:
    return "\x1B[4~";
  case SDLK_PAGEUP:
    return "\x1B[5~";
  case SDLK_PAGEDOWN:
    return "\x1B[6~";
  case SDLK_F1:
    return "\x1B[OP";
  case SDLK_F2:
    return "\x1B[OQ";
  case SDLK_F3:
    return "\x1B[OR";
  case SDLK_F4:
    return "\x1B[OS";
  case SDLK_F5:
    return "\x1B[15~";
  case SDLK_F6:
    return "\x1B[17~";
  case SDLK_F7:
    return "\x1B[18~";
  case SDLK_F8:
    return "\x1B[19~";
  case SDLK_F9:
    return "\x1B[20~";
  case SDLK_F10:
    return "\x1B[21~";
  case SDLK_F11:
    return "\x1B[23~";
  case SDLK_F12:
    return "\x1B[24~";
  default:
    return "";
  }
}
