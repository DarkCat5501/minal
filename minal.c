#include "minal.h"
#define UTF8_IMPL
#include "utf8.h"

extern char** environ;

void minal_default_color_palett(ColorPalette* palette)
{
  palette->bg.rgba = (SDL_Color){ 0x1d, 0x1d, 0x1d, 0xff };
  palette->fg.rgba = (SDL_Color){ 0xfa, 0xfa, 0xfa, 0xff };
  palette->c[0].rgba = (SDL_Color){ 0x00, 0x00, 0x00, 0xff };  // Black
  palette->c[1].rgba = (SDL_Color){ 0x7f, 0x00, 0x00, 0xff };  // Red
  palette->c[2].rgba = (SDL_Color){ 0x00, 0x7f, 0x00, 0xff };  // Green
  palette->c[3].rgba = (SDL_Color){ 0x7f, 0x7f, 0x00, 0xff };  // Yellow
  palette->c[4].rgba = (SDL_Color){ 0x00, 0x00, 0x7f, 0xff };  // Blue
  palette->c[5].rgba = (SDL_Color){ 0x7f, 0x00, 0x7f, 0xff };  // Magenta
  palette->c[6].rgba = (SDL_Color){ 0x00, 0x7f, 0x7f, 0xff };  // Cyan
  palette->c[7].rgba = (SDL_Color){ 0x7f, 0x7f, 0x7f, 0xff };  // Gray
  palette->c[8].rgba = (SDL_Color){ 0x33, 0x33, 0x33, 0xff };  // Dark Gray
  palette->c[9].rgba = (SDL_Color){ 0xff, 0x00, 0x00, 0xff };  // Bright Red
  palette->c[10].rgba = (SDL_Color){ 0x00, 0xff, 0x00, 0xff }; // Bright Green
  palette->c[11].rgba = (SDL_Color){ 0xff, 0xff, 0x00, 0xff }; // Bright Yellow
  palette->c[12].rgba = (SDL_Color){ 0x00, 0x00, 0xff, 0xff }; // Bright Blue
  palette->c[13].rgba = (SDL_Color){ 0xff, 0x00, 0xff, 0xff }; // Bright Magenta
  palette->c[14].rgba = (SDL_Color){ 0x00, 0xff, 0xff, 0xff }; // Bright Cyan
  palette->c[15].rgba = (SDL_Color){ 0xff, 0xff, 0xff, 0xff }; // Bright White
}

void minal_buffer_resize(Minal* m, MinalBuffer* buffer)
{
  buffer->_rect = (SDL_FRect){ .x = 0.f,
                               .y = 0.f,
                               .w = (float)m->window_rect.w,
                               .h = (float)m->window_rect.h };

  buffer->_cursor_max =
    (Cursor){ .col = m->window_rect.w / m->config.cell_width,
              .row = m->window_rect.h / m->config.cell_height };

  // TODO: implement independent font and resize
}

void minal_buffer_new(Minal* m)
{
  MinalBuffer buffer = { 0 };
  buffer.visible = false;
  minal_default_color_palett(&buffer.config.palett);
  vec_append(&m->buffers, buffer);
}

void minal_buffer_append_line(MinalBuffer* buffer)
{
  vec_append(buffer, (Line){ 0 });
}
// expands the buffer until it has the desired cursor coordinate
void minal_buffer_expand(MinalBuffer* buffer, Cursor cursor)
{
  for (size_t start = buffer->len; start <= cursor.row; start++) {
    minal_buffer_append_line(buffer);
  }
  // // TODO: implement UTF8-stride
  // for (size_t start = line->len; start < cursor.col; start++) {
  //   vec_append(line, ' ');
  // }
}

Line* minal_buffer_get_line(MinalBuffer* buffer, Cursor cursor)
{
  assert(cursor.row < buffer->len && "Trying to get a line out of bounds");

  return &buffer->items[cursor.row];
}

void minal_buffer_set_at(MinalBuffer* buffer, Cursor cursor, const char* ch)
{
  minal_buffer_expand(buffer, cursor);
  Line* line = minal_buffer_get_line(buffer, cursor);
  if (!line) {
    fprintf(
      stderr, "Failed to insert at position %zux%zu\b", cursor.row, cursor.col);
    return;
  }

  size_t start = utf8_idxn(line->items, line->len, cursor.col);
  size_t byte_len = strlen(ch);
  if (start != line->len) {
    size_t old_bl = utf8_chrlen(line->items[start]);

    if (old_bl > byte_len) {
      size_t diff = old_bl - byte_len;
      for (size_t i = 0; i < byte_len; i++)
        line->items[start + i] = ch[i];
      memmove(
        &line->items + start + old_bl, &line->items + start + byte_len, diff);
    } else if (old_bl == byte_len) {
      for (size_t i = 0; i < byte_len; i++)
        line->items[start + i] = ch[i];
    } else {
      size_t i = 0;
      for (; i < old_bl; i++)
        line->items[start + i] = ch[i];
      for (; i < byte_len; i++)
        vec_insert(line, ch[i], start + i);
    }
  } else {
    for (size_t i = 0; i < byte_len; i++) {
      vec_append(line, ch[i]);
    }
  }
}

MinalBuffer* minal_current_buffer(Minal* m)
{
  assert(m->buffer_idx < m->buffers.len && "Invalid buffer index");
  return &m->buffers.items[m->buffer_idx];
}

void minal_buffer_auto_scroll(Minal* m)
{
  MinalBuffer* cbuffer = minal_current_buffer(m);

  int height = 0;
  TTF_SetTextWrapWidth(m->text, cbuffer->_rect.w);
  for (size_t i = 0; i < cbuffer->len; i++) {
    Line* line = &cbuffer->items[i];
    int advance = 0;
    if (line->len > 0) {
      TTF_SetTextString(m->text, (char*)line->items, line->len);
      TTF_GetTextSize(m->text, NULL, &advance);
    } else
      advance = m->config.cell_height;
    height += advance;
  }

  if (height > cbuffer->_rect.h) {
    cbuffer->_scroll.y = (float)(cbuffer->_rect.h - height);
  }
}

void minal_buffer_cursor_linefeed(Minal* m)
{
  MinalBuffer* cbuffer = minal_current_buffer(m);
  m->cursor.row++;
  minal_buffer_expand(cbuffer, m->cursor);
}

void minal_buffer_cursor_advance(Minal* m)
{
  MinalBuffer* cbuffer = minal_current_buffer(m);
  Cursor cursor_max = cbuffer->_cursor_max;

  // printf("advance max cursor: %d %d\n", cursor_max.col, cursor_max.row);

  if (m->cursor.col + 1 >= cursor_max.col) {
    m->cursor.col = 0;
    m->cursor.row++;
  } else
    m->cursor.col++;
}

void minal_spawn_shell(Minal* m)
{
  int master_fd;
  int slave_fd;
  if (openpty(&master_fd, &slave_fd, NULL, NULL, NULL) == -1) {
    printf("ERROR: openpty failed: %s\n", strerror(errno));
    exit(1);
  };

  m->shell_pid = fork();
  if (m->shell_pid == 0) {
    login_tty(slave_fd);
    char* defaultshell = getenv("SHELL");
    char cols[50];
    char rows[50];
    char* path = "/usr/bin/bash";
    setenv("SHELL", path, 1);
    // if (defaultshell != NULL)
    //     path = defaultshell;

    sprintf(cols, "%d", 80);
    sprintf(rows, "%d", 24);
    setenv("COLUMNS", cols, 1);
    setenv("LINES", rows, 1);
    setenv("PS1", "$ ", 1);
    // setenv("TERM", "dumb", 1);
    setenv("TERM", "xterm", 1);
    // setenv("TERM", "vt100", 1);

    char* argv[] = { path, NULL };
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

void minal_check_shell(Minal* m)
{
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

void minal_resize_font(Minal* m)
{
  Config* c = &m->config;
  SDL_GetWindowSize(m->window, &m->window_rect.w, &m->window_rect.h);
  assert(
    TTF_SetFontSizeDPI(c->font, c->font_size, c->display_dpi, c->display_dpi));
  assert(TTF_GetStringSize(c->font, " ", 1, &c->cell_width, &c->cell_height));

  for (size_t i = 0; i < m->buffers.len; i++) {
    MinalBuffer* buffer = &m->buffers.items[i];
    minal_buffer_resize(m, buffer);
  }
}

Minal minal_init()
{
  Minal m = { 0 };
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  assert(TTF_Init());

  // default configs
  m.config.font_size = DEFAULT_FONT_SIZE;
  m.config.display_dpi = DEFAULT_DISPLAY_DPI;
  m.window_rect = (SDL_Rect){ .w = 800, .h = 700 };

  minal_spawn_shell(&m);
  m.run = true;

  m.config.font = TTF_OpenFont(FONT_FILE, m.config.font_size);
  assert(m.config.font != NULL && "Could not initialize font");
  m.window = SDL_CreateWindow(
    "Minal", m.window_rect.w, m.window_rect.h, SDL_WINDOW_RESIZABLE);
  assert(m.window != NULL && "Could not initialize window");
  m.rend = SDL_CreateRenderer(m.window, NULL);
  SDL_SetRenderVSync(m.rend, 1);
  assert(m.rend != NULL && "Could not initialize renderer");
  m.text_engine = TTF_CreateRendererTextEngine(m.rend);
  assert(m.text_engine != NULL && "Could not initialize text engine");
  m.text = TTF_CreateText(m.text_engine, m.config.font, "", 0);
  assert(m.text != NULL && "Could not initialize text object");
  // create initial buffer
  minal_buffer_new(&m);
  minal_resize_font(&m);
  return m;
}

void minal_cursor_move(Minal* m, int new_col, int new_row)
{
  m->cursor.col = new_col;
  m->cursor.row = new_row;
}

void minal_write_str(Minal* m, const char* s)
{
  size_t n = strlen(s);
  write(m->master_fd, s, n);
}

void minal_write_char(Minal* m, int c)
{
  write(m->master_fd, &c, 1);
}

int minal_read_nonblock(Minal* m, char* buf, size_t n)
{
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

void line_print(Line* line)
{
  printf("        Line After: {\n");
  printf("         cap:     %zu,\n", line->cap);
  printf("         len:     %zu,\n", line->len);
  printf("         items: [ \n");
  for (size_t j = 0; j < line->len; ++j) {
    uint8_t it = line->items[j];
    printf("                - %08b (%c)\n", it, it);
  }
  printf("     ]}\n");
}

void minal_erase_in_line(Minal* m, size_t opt)
{
  // size_t x = m->cursor.col;
  // size_t y = m->cursor.row;
  // Line* line = &m->screen.items[y];
  // size_t start;
  // size_t end;
  //
  // if (opt == 1 || opt == 2) {
  //   start = 0;
  // } else {
  //   start = line_col2idx(&m->screen.items[y], x);
  // }
  //
  // if (opt == 0 || opt == 2) {
  //   end = line->len - 1;
  // } else {
  //   end = line_col2idx(&m->screen.items[y], x);
  // }
  //
  // memset(line->items + start, ' ', end - start + 1);
}

void minal_erase_in_display(Minal* m, size_t opt)
{
  //   size_t x = m->cursor.col;
  //   size_t y = m->cursor.row;
  //
  //   size_t start = opt == 1 || opt == 2 ? 0 : y;
  //   size_t end = opt == 0 || opt == 2 ? m->config.n_rows - 1 : y;
  //
  //   if (start > end) {
  //     size_t tmp = start;
  //     start = end;
  //     end = tmp;
  //   }
  //
  //   for (size_t row = start; row <= end; ++row) {
  //     if (row == y) {
  //       minal_cursor_move(m, x, row);
  //       minal_erase_in_line(m, opt);
  //     } else {
  //       minal_cursor_move(m, 0, row);
  //       minal_erase_in_line(m, 2);
  //     }
  //   }
  //
  //   if (opt == 2)
  //     minal_cursor_move(m, 0, 0);
  //   else
  //     minal_cursor_move(m, x, y);
}

void minal_parse_ansi(Minal* m, StringView* bytes)
{
  MinalBuffer* cbuffer = minal_current_buffer(m);
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

    int argv[10] = { -1 };
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
        size_t new_row = m->cursor.row < opt ? 0 : m->cursor.row - opt;
        m->cursor.row = new_row;
      }; break;

      case CURSOR_DOWN: {
        int opt = argc > 0 ? argv[0] : 1;
        size_t new_row = (m->cursor.row + opt >= cbuffer->len)
                           ? cbuffer->len - 1
                           : m->cursor.row + opt;
        m->cursor.row = new_row;
      }; break;

      case CURSOR_FORWARD: {
        int opt = argc > 0 ? argv[0] : 1;

        // size_t new_col;
        // if (m->cursor.col + opt >= m->config.n_cols) {
        //   new_col = m->config.n_cols - 1;
        // } else {
        //   new_col = m->cursor.col + opt;
        // }
        // minal_cursor_move(m, new_col, m->cursor.row);
      }; break;

      case CURSOR_BACK: {
        int opt = argc > 0 ? argv[0] : 1;
        size_t new_col = m->cursor.col < opt ? new_col: m->cursor.col - opt;
        m->cursor.col = new_col;
      }; break;

      case CURSOR_POSITION: {
        int opt1 = argc > 0 ? argv[0] : 1;
        int opt2 = argc > 1 ? argv[1] : 1;

        size_t new_col = MIN(MAX(0, opt1 - 1), cbuffer->_cursor_max.col - 1);
        size_t new_row = MIN(MAX(0, opt2 - 1), cbuffer->len - 1);
        m->cursor.col = new_col;
        m->cursor.row = new_row;
      }; break;

      case ERASE_IN_DISPLAY: {
        for(size_t i=0;i<cbuffer->len;i++) {
          Line* line = minal_buffer_get_line(cbuffer, (Cursor){ .row = i});
          vec_free(line);
        }
        cbuffer->len = 0;
        cbuffer->_scroll.y = 0.0;
      }; break;

      case ERASE_IN_LINE: {
        printf("called erase line\n");
        size_t opt;
        if (argv[0] == -1) {
          opt = 0;
        }
        minal_erase_in_line(m, opt);
      }; break;
    }
  }
}

void minal_receiver(Minal* m)
{
  size_t offset = 0;
  char _buf[_32K];
  while (true) {
    assert(offset < _32K && "Need a bigger buffer");
    int n = minal_read_nonblock(m, _buf + offset, _32K - offset);
    if (n == 0)
      break;
    offset += n;
  }
  StringView view = (StringView){ .data = _buf, .len = offset };

  if (!view.len)
    return;
  // printf("input: %.*s\n", (int)view.len, view.data);

  while (view.len > 0) {
    uint8_t ch = (uint8_t)sv_chop_left(&view);
    if (!ch)
      continue;
    //
    // if (ch == '\n')
    //   printf("new line -------------\n");
    // else if (ch == '\r')
    //   printf("<---------------------\n");
    // else if (ch == '\x1b')
    //   printf("input: 0x1b \"ESC\"\n");
    // else
    //   printf("input: 0x%02X \"%c\"\n", ch, ch);

    switch (ch) {
      case BELL:
        break;
      case LINEFEED: {
        minal_buffer_cursor_linefeed(m);
      } break;
      case CARRIAGERET: {
        m->cursor.col = 0;
      } break;
      case DEL:
      case BACKSPACE: {
        Cursor* cursor = &m->cursor;
        if (!(cursor->col > 0 || cursor->row > 0))
          continue;

        if (cursor->col > 0)
          cursor->col--;
        else if (cursor->row > 0) {
          MinalBuffer* cbuffer = minal_current_buffer(m);
          cursor->row--;
          Line* line = minal_buffer_get_line(cbuffer, *cursor);
          cursor->col = utf8_strnlen((const char*)line->items, line->len) - 1;
        }
      } break;
      case ESC: {
        minal_parse_ansi(m, &view);
        continue;
      } break;
      default: {
        size_t u_len = utf8_chrlen(ch);
        uint8_t data[5] = { 0 };
        size_t utf8cur = 0;
        for (size_t i = 0; i < u_len; i++) {
          assert(i == 0 || (ch >> 6) == 0b10);
          data[i] = ch;
          if (i < u_len - 1)
            ch = (uint8_t)sv_chop_left(&view);
        }

        MinalBuffer* cbuffer = minal_current_buffer(m);
        minal_buffer_set_at(cbuffer, m->cursor, data);
        minal_buffer_cursor_advance(m);
      } break;
    }
  }

  minal_buffer_auto_scroll(m);
}

void minal_transmitter(Minal* m)
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
    switch (event.type) {
      case SDL_EVENT_MOUSE_WHEEL: {
        MinalBuffer* buffer = minal_current_buffer(m);
        buffer->_scroll.y += event.wheel.y * m->config.cell_height * 1.5;
      } break;
      case SDL_EVENT_QUIT:
        m->run = false;
        return;
      case SDL_EVENT_WINDOW_RESIZED:
        return minal_resize_font(m);
      case SDL_EVENT_KEY_DOWN: {
        if (event.key.key == SDLK_ESCAPE) {
          m->run = false;
          return;
        }
        SDL_Keycode k = event.key.key;
        switch (k) {
          case SDLK_F1:
            return minal_write_str(m, "𒀀");
          default: {
            const char* code = SDLK_to_ansicode(k);
            if (code)
              minal_write_str(m, code);
          }; break;
        }
      } break;
      case SDL_EVENT_TEXT_INPUT: {
        return minal_write_str(m, event.text.text);
      }
    }
}

void minal_run(Minal* m)
{
  assert(SDL_StartTextInput(m->window) && "Could not start text input mode");
  while (m->run) {
    minal_check_shell(m);
    minal_receiver(m);
    minal_transmitter(m);

    SDL_SetRenderDrawColor(m->rend, 0, 0, 0, 0xff);
    SDL_RenderClear(m->rend);

    SDL_FRect cur = (SDL_FRect){
      .x = m->cursor.col * m->config.cell_width,
      .y = 0,
      .w = m->config.cell_width,
      .h = m->config.cell_height,
    };

    for (size_t i = 0; i < m->buffers.len; i++) {
      MinalBuffer* buffer = &m->buffers.items[i];
      SDL_Color bg = buffer->config.palett.bg.rgba;
      SDL_SetRenderDrawColor(m->rend, bg.r, bg.g, bg.b, bg.a);
      SDL_RenderFillRect(m->rend, &buffer->_rect);

      TTF_SetTextWrapWidth(m->text, buffer->_rect.w);
      int next_h = (int)buffer->_scroll.y;
      // TODO: draw the buffer
      for (size_t line_idx = 0; line_idx < buffer->len; line_idx++) {
        Line* line = &buffer->items[line_idx];

        if (line_idx == m->cursor.row)
          cur.y = next_h;
        if (line->len > 0) {
          TTF_SetTextString(m->text, (char*)line->items, line->len);
          int tw, th;
          TTF_GetTextSize(m->text, &tw, &th);
          TTF_SetTextColor(m->text, 0xfa, 0xfa, 0xfa, 0xff);
          TTF_DrawRendererText(m->text, 0, next_h);
          next_h += th;
        } else
          next_h += m->config.cell_height;
      }
    }
    SDL_SetRenderDrawColor(m->rend, 0xff, 0x0a, 0xff, 0xff);
    SDL_RenderFillRect(m->rend, &cur);
    SDL_RenderPresent(m->rend);
  }
}

void minal_finish(Minal* m)
{
  // for (size_t i = 0; i < m->screen.len; ++i) {
  //   vec_free(&m->screen.items[i]);
  // }
  // vec_free(&m->screen);
  // sb_free(&m->display_str);

  assert(SDL_StopTextInput(m->window));
  TTF_CloseFont(m->config.font);

  TTF_DestroyRendererTextEngine(m->text_engine);
  TTF_DestroyText(m->text);

  SDL_DestroyRenderer(m->rend);
  SDL_DestroyWindow(m->window);

  TTF_Quit();
  SDL_Quit();
}

int main(void)
{
  Minal m = minal_init();
  minal_run(&m);
  minal_finish(&m);
  return 0;
}

const char* SDLK_to_ansicode(SDL_Keycode key)
{
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
      return 0;
  }
}
