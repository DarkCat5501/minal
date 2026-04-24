#include "../ansi.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
  const char* names[] = {"black", "red", "green", "yellow", "blue", "magenta", "cyan", "white"};
  
printf("Testing SGR color codes:\n");
  for(int i = 0; i < 8; i++) {
    ANSI_Cmd cmd = {0};
    char buf[32];
    snprintf(buf, sizeof(buf), "\e[%d;%dm", 30+i, 40+i);
    
    printf("  %s (fg=%d bg=%d): ", names[i], 30+i, 40+i);
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    if(!ok) { printf("FAILED\n"); continue; }
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
  
  // Test bright colors (90-97, 100-107)
  printf("\nTesting bright colors:\n");
  for(int i = 0; i < 8; i++) {
    ANSI_Cmd cmd = {0};
    char buf[32];
    snprintf(buf, sizeof(buf), "\e[%d;%dm", 90+i, 100+i);
    
    printf("  bright %s (fg=%d bg=%d): ", names[i], 90+i, 100+i);
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    if(!ok) { printf("FAILED\n"); continue; }
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
  
  // Test RGB true color (foreground and background combined)
  printf("\nTesting RGB true color (fg + bg):\n");
  {
    ANSI_Cmd cmd = {0};
    const char* buf = "\e[38;2;255;128;0;48;2;0;128;255m";  // orange fg, cyan bg
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    printf("  fg RGB 255,128,0 + bg RGB 0,128,255: ");
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
  {
    ANSI_Cmd cmd = {0};
    const char* buf = "\e[38;2;200;50;100;48;2;50;100;200m";  // purple-ish
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    printf("  fg RGB 200,50,100 + bg RGB 50,100,200: ");
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
{
    ANSI_Cmd cmd = {0};
    const char* buf = "\e[38;2;255;255;255;48;2;0;0;0m";  // white fg, black bg
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    printf("  fg RGB 255,255,255 + bg RGB 0,0,0: ");
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
  
  // Test underline color (58) - 16 color (basic colors 0-15)
  printf("\nTesting underline color (58) - basic 16 color:\n");
  {
    ANSI_Cmd cmd = {0};
    const char* buf = "\e[58;5;5m";  // underline 16 color 5 (magenta)
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    printf("  ul color 5: ");
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
  
  // Test underline color (58) - direct 16 color (50-57 is NOT valid, just testing color 0-15)
  printf("\nTesting underline color (58) - direct 58;n (range 0-15):\n");
  {
    ANSI_Cmd cmd = {0};
    const char* buf = "\e[58;12m";  // using a smaller n to test direct 16-color
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    printf("  ul color 12 (bright): ");
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
  
  // Test underline color (58) - 256 color (cube)
  printf("\nTesting underline color (58) - 256 color (cube):\n");
  {
    ANSI_Cmd cmd = {0};
    const char* buf = "\e[58;5;196m";  // underline 256 color (cube)
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    printf("  ul color 196: ");
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
  
  // Test underline color (58) - 256 color (grayscale)
  printf("\nTesting underline color (58) - 256 color (grayscale):\n");
  {
    ANSI_Cmd cmd = {0};
    const char* buf = "\e[58;5;242m";  // underline 256 color (grayscale)
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    printf("  ul color 242: ");
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
  
  // Test underline color (58) - RGB
  printf("\nTesting underline color (58) - RGB:\n");
  {
    ANSI_Cmd cmd = {0};
    const char* buf = "\e[58;2;255;128;0m";  // underline orange
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    printf("  ul RGB 255,128,0: ");
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
  
  // Test all 256 colors
  printf("\nTesting all 256 colors:\n");
  for(int c = 0; c < 256; c++) {
    ANSI_Cmd cmd = {0};
    char buf[32];
    snprintf(buf, sizeof(buf), "\e[38;5;%dm", c);
    
    bool ok = ansi_decode_cmd(&cmd, buf, strlen(buf));
    if(!ok) { printf("FAILED %d\n", c); continue; }
    ansi_debug_cmd(&cmd);
    printf("\n");
  }
  
  return 0;
}
