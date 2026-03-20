#include "minal.h"
#include "carrlib/vec.h"

bool is_utf8_head(uint8_t ch) 
{
    return ((ch >> 7) & 1) == 1 &&
            (ch >> 6)  != 0b10;
}

size_t utf8_chrlen(char ch)
{
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


void minal_spawn_shell(Minal* m) 
{
    int master_fd;
    int slave_fd;
    if (openpty(&master_fd, &slave_fd, NULL, NULL, NULL) == -1) {
        printf("ERROR: openpty failed: %s\n", strerror(errno));
        exit(1);
    };

    if (fork() == 0) {
        login_tty(slave_fd);
        char* env[] = {"TERM=DUMB" ,"PS1=$ ", NULL};
        // char* env[] = {"TERM=vt100", "PS1=$ ",  NULL};
        // char* env[] = {"TERM=xterm-256color" ,"PS1=$ ", NULL};
        char* path = "/bin/bash";
        char* argv[] = { path, NULL };
        if (execve(path, argv, env) == -1) {
            printf("ERROR: execv: %s\n", strerror(errno));
            exit(1);
        };
        usleep(2000);
        exit(0);
    } 

    m->master_fd = master_fd;
    m->slave_fd  = slave_fd;
}

Minal minal_init()
{
    Minal m = {0};
    minal_spawn_shell(&m);
    m.run = true;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    assert(TTF_Init());

    // default configs
    m.config.font_size   = DEFAULT_FONT_SIZE;
    m.config.n_cols      = DEFAULT_N_COLS;
    m.config.n_rows      = DEFAULT_N_ROWS;
    m.config.display_dpi = DEFAULT_DISPLAY_DPI;

    m.config.font = TTF_OpenFont(FONT_FILE, m.config.font_size);
    assert(m.config.font != NULL);
    assert(TTF_SetFontSizeDPI(m.config.font, m.config.font_size, m.config.display_dpi, m.config.display_dpi));

    assert(TTF_GetStringSize(m.config.font, "A", 1, &m.config.cell_width, &m.config.cell_height));
    m.config.window_width  = m.config.cell_width  * m.config.n_cols;
    m.config.window_height = m.config.cell_height * m.config.n_rows;

    m.window = SDL_CreateWindow("Minal", m.config.window_width, m.config.window_height, 0);
    assert(m.window != NULL);

    m.rend = SDL_CreateRenderer(m.window, NULL);
    assert(m.rend != NULL);

    m.text_engine = TTF_CreateRendererTextEngine(m.rend);
    assert(m.text_engine != NULL);

    m.text = TTF_CreateText(m.text_engine, m.config.font, "", 0);
    assert(m.text != NULL);

    Display display = {0};
    vec_expandto(&display, m.config.n_rows);
    for (int i = 0; i < m.config.n_rows; ++i) {
        Line l = {0};
        vec_expandto(&l, m.config.n_cols);
        memset(l.items, 0, l.cap * sizeof(char));
        vec_append(&display, l);
    }
    m.display     = display;
    m.display_str = sb_with_cap(4 * m.config.n_rows * m.config.n_cols);


    m.fg_color = (SDL_Color) {
        .r = 220,
        .g = 220,
        .b = 220,
        .a = 255,
    };
    m.bg_color = (SDL_Color) {
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255,
    };            

    return m;
}

SDL_FRect minal_cursor_to_rect(Minal* m)
{
    return (SDL_FRect) {
        .x = m->cursor.col * m->config.cell_width,
        .y = m->cursor.row * m->config.cell_height,
        .w = m->config.cell_width, 
        .h = m->config.cell_height,
    };
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


int minal_read_nonblock(Minal* m, char* buf)
{
    int oldf = fcntl(m->master_fd, F_GETFL, 0);
    assert(oldf != -1);
    int ret = fcntl(m->master_fd, F_SETFL, oldf | O_NONBLOCK);
    assert(ret != -1);
    int n = read(m->master_fd, buf, _32K);
    if (n == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            printf("ERROR: read nonblock: %s\n", strerror(errno));
            exit(1);
        }
        return 0;
    }
    return n;
}

size_t minal_line_col2idx(Minal* m, size_t col, size_t row)
// col:  0  1   2            3  4   5      6  7   8         9
//     [ _, _, {_, _, _, _}, _, _, {_, _}, _, _, {_, _, _}, _ ]
// idx:  0  1   2  3  4  5   6  7   8  9  10 11  12 13 14  15
//              ^
//           start = 2
//           start + size = start + 4 = 2 + 4 = 6
//           start + size - size = start
// col = 2 => col2idx = 2
// col = 3 => col2idx = 6
// col = 5 => col2idx = 8
// col = 8 => col2idx = 12
{
    Line l = m->display.items[row];
    if (l.len == 0) return 0;

    size_t i = 0;
    size_t cols = 0;
    while (i < l.len && cols < col) {
        i += utf8_chrlen(l.items[i]);
        cols++;
    }

    return i;
}

size_t minal_line_chars(Minal* m, size_t row)
{
    Line l = m->display.items[row];
    if (l.len == 0) return 0;

    int chars = 0;

    size_t i = 0;
    while (i < l.len) {
        chars++;
        i += utf8_chrlen(l.items[i]);
    }

    return chars;
}

size_t minal_row_size(Minal* m, size_t row)
{
    return m->display.items[row].len;
}

uint8_t minal_at(Minal *m, size_t col, size_t row)
{
    assert(row >= 0 && row < m->display.len);

    size_t idx = minal_line_col2idx(m, col, row);
    assert(idx >= 0 && idx < m->display.items[row].len);

    return m->display.items[row].items[idx];
}

void minal_append(Minal* m, size_t row, char c)
{
    assert(row >= 0 && row < m->display.len);
    Line r = m->display.items[row];
    vec_append(&m->display.items[row], (uint8_t)c);
}

void minal_insert_at(Minal* m, size_t col, size_t row, char c)
{
    assert(row >= 0 && row < m->display.len);
    Line r = m->display.items[row];
    assert(col >= 0 && col < r.len);
    size_t idx = minal_line_col2idx(m, col, row);
    if (r.items[idx] == '\0') {
         r.items[idx] = (uint8_t)c;
    } else {
        vec_insert(&r, (uint8_t)c, idx);
    }
}

void minal_insert_at_idx(Minal* m, size_t idx, size_t row, char c)
{
    assert(row >= 0 && row < m->display.len);
    Line r = m->display.items[row];
    assert(idx >= 0 && idx < r.len);
    if (r.items[idx] == '\0') {
         r.items[idx] = (uint8_t)c;
    } else {
        vec_insert(&r, (uint8_t)c, idx);
    }
}

void minal_receiver(Minal* m)
{
    SDL_SetRenderDrawColor(m->rend, m->bg_color.r, m->bg_color.g, m->bg_color.b, m->bg_color.a);
    SDL_RenderClear(m->rend);

    bool isutf8;
    size_t utf8cur;
    size_t utf8rem;
    char buf[_32K];
    while (true) {
        int n = minal_read_nonblock(m, buf);
        if (n == 0) break;

        int i = 0;
        while (i < n) {
            uint8_t ch = buf[i];

            size_t x   = m->cursor.col;
            size_t y   = m->cursor.row;
            Line line  = m->display.items[y];
            size_t idx = minal_line_col2idx(m, x, y);

            if (ch == BELL) {
                i++;
                continue;
            }

            if (ch == LINEFEED) {
                minal_cursor_move(m, 0, y + 1);
                i++;
                continue;
            }

            if (ch == DEL) {
                printf("RECEIVED DEL\n");
            }

            if (ch == BACKSPACE) {
                size_t start = minal_line_col2idx(m, x - 1, y);  
                char head = m->display.items[y].items[start];
                if (is_utf8_head(head)) {
                    size_t n = utf8_chrlen(head);
                    for (size_t j = 1; j <= n; ++j) {
                        m->display.items[y].items[start + n - j] = '\0';
                        m->display.items[y].len--;
                    }
                } else {
                    m->display.items[y].items[start] = '\0';
                    m->display.items[y].len--;
                }


                if (m->cursor.col == 0 && m->cursor.row > 0) {
                    minal_cursor_move(m, m->config.n_cols - 1, m->cursor.row - 1);
                } else {
                    minal_cursor_move(m, m->cursor.col - 1, m->cursor.row);
                }

                i++;
                continue;
            }

            // TODO: handle ANSI escape sequences
            // if (ch == '\x1B') {
            //     continue;
            // }


            if (is_utf8_head(ch)) {
                isutf8 = true;
                size_t chlen = utf8_chrlen(ch);
                utf8rem = chlen - 1;
                utf8cur = 0;
            } else {
                if (utf8rem > 0) {
                    assert((ch >> 6) == 0b10);
                }
            }

            if (isutf8) {
                idx = idx + utf8cur;
            }

            minal_append(m, y, ch);
            // TODO: handle arbitrary position insertions
            // if (idx >= line.len) {
            //     int n = idx - line.len;
            //     for (int i = 0; i < n; ++i) {
            //         minal_append(m, y, ' ');
            //     }
            //     printf("Appending Byte '0b%08b'\n", ch) ;
            // } else {
            //     printf("Inserting Byte '0b%08b' at %zu\n", ch, idx);
            //     minal_insert_at_idx(m, idx, y, ch);
            // }

            if (isutf8) {
                if (utf8rem > 0) {
                    utf8cur++;
                    utf8rem--;
                } else {
                    utf8cur = 0;
                    isutf8 = false;
                }
            }

            if (!isutf8) {
                if (m->cursor.col == m->config.n_cols - 1) {
                    minal_cursor_move(m, 0, m->cursor.row + 1);
                } else {
                    minal_cursor_move(m, m->cursor.col + 1, m->cursor.row);
                }
            }

            i++;
        }
    }

    m->display_str.len = 0;
    for (size_t row = 0; row < m->display.len; row++) {
        Line l = m->display.items[row];
        sb_concat(&m->display_str, l.items);
        sb_append(&m->display_str, '\n');
    }

    TTF_SetTextString(m->text, m->display_str.data, m->display_str.len);
    SDL_SetRenderDrawColor(m->rend, m->fg_color.r, m->fg_color.r, m->fg_color.r, m->fg_color.a);
    TTF_DrawRendererText(m->text, 0, 0);
}

void minal_transmitter(Minal* m, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT) { 
        m->run = false;
        return;
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
                const char* code = SDLK_to_ansicode(k);
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

void minal_main(Minal* m)
{
    assert(SDL_StartTextInput(m->window));
    while (m->run) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            minal_transmitter(m, &event);
        }

        minal_receiver(m);

        SDL_SetRenderDrawColor(m->rend, m->fg_color.r, m->fg_color.g, m->fg_color.b, m->fg_color.a);
        SDL_FRect cur = minal_cursor_to_rect(m);
        SDL_RenderFillRect(m->rend, &cur);

        SDL_RenderPresent(m->rend);

        SDL_Delay(1000 / FPS);
    }
}

void minal_finish(Minal *m)
{
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


int main(void)
{
    Minal m = minal_init();
    minal_main(&m);
    return 0;
}

const char* SDLK_to_ansicode(SDL_Keycode key)
{ 
    switch (key) {
        case SDLK_RETURN:    return "\n";
        case SDLK_TAB:       return "\t";
        case SDLK_UP:        return "\x1B[A";
        case SDLK_DOWN:      return "\x1B[B";
        case SDLK_RIGHT:     return "\x1B[C";
        case SDLK_LEFT:      return "\x1B[D";
        case SDLK_HOME:      return "\x1B[1~";
        case SDLK_INSERT:    return "\x1B[2~";
        case SDLK_DELETE:    return "\x1B[3~";
        case SDLK_END:       return "\x1B[4~";
        case SDLK_PAGEUP:    return "\x1B[5~";
        case SDLK_PAGEDOWN:  return "\x1B[6~";
        case SDLK_F1:        return "\x1B[OP";
        case SDLK_F2:        return "\x1B[OQ";
        case SDLK_F3:        return "\x1B[OR";
        case SDLK_F4:        return "\x1B[OS";
        case SDLK_F5:        return "\x1B[15~";
        case SDLK_F6:        return "\x1B[17~";
        case SDLK_F7:        return "\x1B[18~";
        case SDLK_F8:        return "\x1B[19~";
        case SDLK_F9:        return "\x1B[20~";
        case SDLK_F10:       return "\x1B[21~";
        case SDLK_F11:       return "\x1B[23~";
        case SDLK_F12:       return "\x1B[24~";
        default: return "";
    }
}

