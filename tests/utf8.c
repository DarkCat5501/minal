#include <stdio.h>
#include <string.h>
#define UTF8_IMPL
#include "../utf8.h"

int main(int argc, char* argv[])
{

  const char* data = "maçã※𒀀";
  size_t len = strlen(data);
  printf("len %zu\n", len);

  size_t utf_len = utf8_strnlen(data, len);
  printf("utf len %zu\n", utf_len);
  

  size_t ch = utf8_idxn(data, len, 5);
  printf("index 2 = %zu\n", ch);

  return 0;
}
