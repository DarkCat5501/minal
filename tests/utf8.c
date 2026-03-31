#include <stdio.h>
#include <string.h>
#define UTF8_IMPL
#include "../utf8.h"

int main(int argc, char* argv[])
{

  const char* data = "maçã※𒀀";
  size_t len = strlen(data);
  printf("len %zu\n", len);

  uint32_t u32char = 0x1F64A; //macaco 
  // utf8_buf_to_utf32_char(&u32char,&data[9],2,NULL);

  size_t utf_len = utf8_strnlen(data, len);
  printf("utf len %zu\n", utf_len);
  
  printf("utf32 [0]  = 0x%06X\n", u32char);

  utf8_char_t u8char = utf32_char_to_utf8_char(u32char);

  printf("utf8 [0]  = %.*s\n", u8char.len, u8char.buf);

  size_t ch = utf8_idxn(data, len, 5);
  printf("index 2 = %zu\n", ch);

  return 0;
}
