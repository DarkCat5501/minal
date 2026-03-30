#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool utf8_is_head(uint8_t ch);
size_t utf8_chrlen(char ch);
size_t utf8_strlen(const char* str);
size_t utf8_strnlen(const char* str, size_t n);
size_t utf8_idxn(const char* str, size_t n, size_t i);

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

#endif // UTF8_IMPL
