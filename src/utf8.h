#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

bool utf8_is_head(uint8_t ch);
size_t utf8_chrlen(char ch);
size_t utf8_strlen(const char* str);
size_t utf8_strnlen(const char* str, size_t n);
size_t utf8_idxn(const char* str, size_t n, size_t i);
int utf8_buf_to_utf32_char(uint32_t* out_char32, const char* utf8_buf, size_t len, int* opt_error);

typedef struct utf8_char_t { char buf[4]; uint16_t len; } utf8_char_t;
uint32_t utf8_char_to_utf32_char(const utf8_char_t char_utf8, int* opt_error);
utf8_char_t utf32_char_to_utf8_char(const uint32_t char32);

#if defined(UTF8_IMPL)

bool utf8_is_head(uint8_t ch)
{
  return ((ch >> 7) & 1) == 1 && (ch >> 6) != 0b10;
}

size_t utf8_chrlen(char ch)
{
  uint8_t b = (uint8_t)ch;
  size_t size = 1;
  if (b >> 7 == 0x00)
    return 1;
  else if (b >> 5 == 0x06)
    return 2;
  else if (b >> 4 == 0x0e)
    return 3;
  else if (b >> 3 == 0x1e)
    return 4;
  return 1;
}

size_t utf8_strnlen(const char* str, size_t n)
{
  size_t utf_len = 0;
  for (size_t i = 0; i < n && str[i] != 0; i += utf8_chrlen(str[i])) {
    utf_len++;
  }
  return utf_len;
}

size_t utf8_strlen(const char* str)
{
  size_t utf_len = 0, i = 0;
  while (str[i] != 0) {
    size_t len = utf8_chrlen(str[i]);
    i += len;
    utf_len++;
  }
  return utf_len;
}

size_t utf8_idxn(const char* str, size_t n, size_t index)
{
  size_t uix = 0, i = 0;
  for (;(uix < index) && (i < n) && (str[i] != 0); i += utf8_chrlen(str[i])) {
    uix++;
  }
  return i;
}

// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
// DOC:
// int utf8_buf_to_utf32_char(uint32_t* out_char32, const char* utf8_buf, const char* utf8_buf_end, int* opt_error);
//      convert utf8 to a 32-bit unicode character
//      return number of bytes processed from the input stream
//      optional error argument is provided
//      regardless of the error argument the function will never return 0
//      at least 1 byte will always be skipped
//
// int c_utf32_char_to_utf8_buf(char* out_utf8_buf, const char* utf8_buf_end, const uint32_t char32);
//      convert a 32-bit unicode character to utf8 string
//      return number of bytes written to buf or 0 on error (invalid unicode char, or buf too small)
//      will not write on utf8_buf_end or beyond
//
// typedef struct utf8_char_t {
//     char buf[5];
//     uint16_t len;
// } utf8_char;
//
// int utf8_char_to_buf(char* out_utf8_buf, const char* utf8_buf_end, utf8_char_t char_utf8);
//      write a utf8_char_t to a buffer
//      returns number of bytes written on 0 if buffer is not big enough
//      no bytes will be written if the buffer is not big enough
//
// utf8_char_t c_utf32_char_to_utf8_char(const uint32_t char32);
//      convert a 32-bit unicode character to a utf8 character
//      returns a char with len = 0 on error (invalid unicode char)
//
// uint32_t utf8_char_to_utf32_char(const utf8_char_t char_utf8, int* opt_error);
//      conver a utf8 character to a 32-but unicode character
//      optional error argument is provided
//
// int utf8_buf_to_utf32_char_b(uint32_t* out_char32, const char* utf8_buf, int* opt_error);
//      branchless convert utf8 to a 32-bit unicode character
//      ! utf8_buf must point to at least 4 bytes with zeroes after the end of the string
//      return number of bytes processed from the output stream
//      optional error argument is provided
//      regardless of the error argument the function will never return 0
//      at least 1 byte will always be skipped


const uint32_t C_UTF32_MAX_CODEPOINT = 0x10FFFF;
const uint32_t C_UTF32_INVALID_CODEPOINT = 0xFFFD;
// based on https://github.com/skeeto/branchless-utf8 by Christopher Wellons
int utf8_buf_to_utf32_char_b(uint32_t* out_char32, const char* utf8_buf, int* opt_error) {
    static const char lengths[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0
    };
    static const int masks[] = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
    static const int shiftc[] = {0, 18, 12, 6, 0};

    const uint8_t* const s = (const uint8_t*)utf8_buf;
    int len = lengths[s[0] >> 3];

    /* Assume a four-byte character and load four bytes. Unused bits are
     * shifted out.
     */
    *out_char32 = (uint32_t)(s[0] & masks[len]) << 18;
    *out_char32 |= (uint32_t)(s[1] & 0x3f) << 12;
    *out_char32 |= (uint32_t)(s[2] & 0x3f) << 6;
    *out_char32 |= (uint32_t)(s[3] & 0x3f) << 0;
    *out_char32 >>= shiftc[len];

    if (opt_error) {
        static const uint32_t mins[] = {4194304, 0, 128, 2048, 65536};
        static const int shifte[] = {0, 6, 4, 2, 0};
        int* const e = opt_error;
        const uint32_t c = *out_char32;
        /* Accumulate the various error conditions. */
        *e = (c < mins[len]) << 6; // non-canonical encoding
        *e |= ((c >> 11) == 0x1b) << 7;  // surrogate half?
        *e |= (c > C_UTF32_MAX_CODEPOINT) << 8;  // out of range?
        *e |= (s[1] & 0xc0) >> 2;
        *e |= (s[2] & 0xc0) >> 4;
        *e |= (s[3]) >> 6;
        *e ^= 0x2a; // top two bits of each tail byte correct?
        *e >>= shifte[len];
    }

    return len + !len;
}

uint32_t utf8_char_to_utf32_char(const utf8_char_t char_utf8, int* opt_error) {
    uint32_t ret;
    utf8_buf_to_utf32_char_b(&ret, char_utf8.buf, opt_error);
    return ret;
}

// Based on stb_to_utf8() from github.com/nothings/stb/
utf8_char_t utf32_char_to_utf8_char(const uint32_t char32) {
    utf8_char_t ret = {0};
    if (char32 < 0x80)
    {
        ret.buf[0] = (char)char32;
        ret.len = 1;
    }
    else if (char32 < 0x800)
    {
        ret.buf[0] = (char)(0xc0 + (char32 >> 6));
        ret.buf[1] = (char)(0x80 + (char32 & 0x3f));
        ret.len = 2;
    }
    else if (char32 < 0x10000)
    {
        ret.buf[0] = (char)(0xe0 + (char32 >> 12));
        ret.buf[1] = (char)(0x80 + ((char32 >> 6) & 0x3f));
        ret.buf[2] = (char)(0x80 + ((char32) & 0x3f));
        ret.len = 3;
    }
    else if (char32 <= 0x10FFFF)
    {
        ret.buf[0] = (char)(0xf0 + (char32 >> 18));
        ret.buf[1] = (char)(0x80 + ((char32 >> 12) & 0x3f));
        ret.buf[2] = (char)(0x80 + ((char32 >> 6) & 0x3f));
        ret.buf[3] = (char)(0x80 + ((char32) & 0x3f));
        ret.len = 4;
    }
    return ret;
}

int utf8_char_to_buf(char* out_utf8_buf, size_t len, utf8_char_t char_utf8) {
    if (len < char_utf8.len) return 0;
    for(size_t i=0;i<char_utf8.len;i++) out_utf8_buf[i] = char_utf8.buf[i];
    return char_utf8.len;
}

int utf8_buf_to_utf32_char(uint32_t* out_char32, const char* utf8_buf, size_t len, int* opt_error) {
    char s[4] = {0};
    size_t u_len = len < 4 ? len: 4;
    for(size_t i=0;i<u_len;i++) s[i] = utf8_buf[i];
    return utf8_buf_to_utf32_char_b(out_char32, s, opt_error);
}

int utf32_char_to_utf8_buf(char* out_utf8_buf, size_t len, const uint32_t char32) {
    utf8_char_t utf8_char = utf32_char_to_utf8_char(char32);
    return utf8_char_to_buf(out_utf8_buf, len, utf8_char);
}

#endif // UTF8_IMPL
