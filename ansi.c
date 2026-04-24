#include "ansi.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

static char* C1_Control__InputMap[] = {
  // Feeler control characters
  [C1_Control_IND] =        "\033D",   // move cursor down one line
  [C1_Control_NEL] =        "\033E",   // move cursor to beginning of next line
  [C1_Control_HTS] =        "\033H",   // set tab stop at current column
  [C1_Control_RI ] =        "\033M",   // move cursor up one line
  [C1_Control_SS2] =        "\033N",   // invoke G2 for next character only
  [C1_Control_SS3] =        "\033O",   // invoke G3 for next character only
  [C1_Control_DCS] =        "\033P",   // device control sequence
  [C1_Control_SPA] =        "\033V",   // start of guarded area
  [C1_Control_EPA] =        "\033W",   // end of guarded area
  [C1_Control_SOS] =        "\033X",   // generic string sequence
  [C1_Control_RTI] =        "\033Z",   // return to index (move cursor up)
  [C1_Control_CSI] =        "\033[",   // control sequence introducer
  [C1_Control_ST ] =        "\033\\",  // string terminator
  [C1_Control_OSC] =        "\033]",   // operating system command
  [C1_Control_APC] =        "\033_",   // application program command

  // VT100 special (2-byte after ESC)
  [C1_Control_VT100_SP]  = "\033 ",   // space
  [C1_Control_VT100_DEC] = "\033#",   // DEC-specific control
  [C1_Control_VT100_SEL] = "\033%",   // selection control
  [C1_Control_VT100_G0]  = "\033(",   // designate G0 character set
  [C1_Control_VT100_G1]  = "\033)",   // designate G1 character set
  [C1_Control_VT100_G2]  = "\033*",   // designate G2 character set
  [C1_Control_VT100_G3]  = "\033+",   // designate G3 character set

  // Special VT100 sequences
  [C1_Control_Special_DECSC] =  "\0337",  // save cursor position
  [C1_Control_Special_DECRC] =  "\0338",  // restore cursor position
  [C1_Control_Special_DECPAM]=  "\033=",  // application keypad mode
  [C1_Control_Special_DECPNM]=  "\033>",  // normal keypad mode
  [C1_Control_Special_CTLLCS]=  "\033F",  // cursor to lower-left corner
  [C1_Control_Special_RIS]   =  "\033c",  // full reset
  [C1_Control_Special_ML]    =  "\033l",  // memory lock
  [C1_Control_Special_MU]    =  "\033m",  // memory unlock
  [C1_Control_Special_LS2]   =  "\033n",  // lock shift 2
  [C1_Control_Special_LS3]   =  "\033o",  // lock shift 3
  [C1_Control_Special_LS3R]  =  "\033|",  // lock shift 3 right
  [C1_Control_Special_LS2R]  =  "\033}",  // lock shift 2 right
  [C1_Control_Special_LS1R]  =  "\033~",  // lock shift 1 right
};
static_assert(sizeof(C1_Control__InputMap)/sizeof(C1_Control__InputMap[0]) == C1_Control__Length,"Unmapped C1_Control InputMap");

static char* C1_Control__ToString[] = {
  [C1_Control_IND]           =  "IND",
  [C1_Control_NEL]           =  "NEL",
  [C1_Control_HTS]           =  "HTS",
  [C1_Control_RI ]           =  "RI",
  [C1_Control_SS2]           =  "SS2",
  [C1_Control_SS3]           =  "SS3",
  [C1_Control_DCS]           =  "DCS",
  [C1_Control_SPA]           =  "SPA",
  [C1_Control_EPA]           =  "EPA",
  [C1_Control_SOS]           =  "SOS",
  [C1_Control_RTI]           =  "RTI",
  [C1_Control_CSI]           =  "CSI",
  [C1_Control_ST ]           =  "ST",
  [C1_Control_OSC]           =  "OSC",
  [C1_Control_APC]           =  "APC",
  [C1_Control_VT100_SP]      =  "VT100SP",
  [C1_Control_VT100_DEC]     =  "VT100DEC",
  [C1_Control_VT100_SEL]     =  "VT100SEL",
  [C1_Control_VT100_G0]      =  "VT100G0",
  [C1_Control_VT100_G1]      =  "VT100G1",
  [C1_Control_VT100_G2]      =  "VT100G2",
  [C1_Control_VT100_G3]      =  "VT100G3",
  //special
  [C1_Control_Special_DECSC] =  "VT100-DECSC",
  [C1_Control_Special_DECRC] =  "VT100-DECRC",
  [C1_Control_Special_DECPAM ]= "VT100-DECPAM",
  [C1_Control_Special_DECPNM ]= "VT100-DECPNM",
  [C1_Control_Special_CTLLCS ]= "VT100-CTLLCS",
  [C1_Control_Special_RIS]   =  "VT100-RIS",
  [C1_Control_Special_ML]    =  "VT100-ML",
  [C1_Control_Special_MU]    =  "VT100-MU",
  [C1_Control_Special_LS2]   =  "VT100-LS2",
  [C1_Control_Special_LS3]   =  "VT100-LS3",
  [C1_Control_Special_LS3R]  =  "VT100-LS3R",
  [C1_Control_Special_LS2R]  =  "VT100-LS2R",
  [C1_Control_Special_LS1R]  =  "VT100-LS1R",
};
static_assert(sizeof(C1_Control__ToString)/sizeof(C1_Control__ToString[0]) == C1_Control__Length,"Unmapped C1_Control ToString");

static char* VT100_CharSet__InputMap[] = {
  [VT100_CharSet_Special_LineDrawing] = "0",
  [VT100_CharSet_UnitedKingdom]       = "A",
  [VT100_CharSet_UnitedStates]        = "B",
  [VT100_CharSet_Dutch]               = "4",
  [VT100_CharSet_Finnish]             = "C5",
  [VT100_CharSet_French]              = "R",
  [VT100_CharSet_FrenchCanadian]      = "Q",
  [VT100_CharSet_German]              = "K",
  [VT100_CharSet_Italian]             = "Y",
  [VT100_CharSet_Norwegian]           = "E6",
  [VT100_CharSet_Spanish]             = "Z",
  [VT100_CharSet_Swidsh]              = "H7",
  [VT100_CharSet_Swiss]               = "= ",
};

static_assert(sizeof(VT100_CharSet__InputMap)/sizeof(VT100_CharSet__InputMap[0]) == VT100_CharSet__Length,"Unmapped VT100_CharSet InputMap");

static C1_Control ansi_match_c1_controls(const char* data, size_t len) {
  if(len < 2) goto invalid;
  for(int i=0;i<C1_Control__Length;i++) {
  if(strncmp(C1_Control__InputMap[i],data,2)==0) return (C1_Control)i;
  }
invalid:
  return C1_Control_Invalid;
}

static VT100_CharSet ansi_match_vt100_charset(const char* data, size_t len) {
  if(len < 2) goto invalid;

  for(int i=0;i<VT100_CharSet__Length;i++) {
  if(strncmp(VT100_CharSet__InputMap[i],data,2)==0) return (VT100_CharSet)i;
  }
invalid:
  return VT100_CharSet_Invalid;
}

void ansi_debug(const char* data, size_t len) {
  for(size_t i=0;i<len;i++) {
  C1_Control ctr = ansi_match_c1_controls(data+i,len-i);
  if(ctr != C1_Control_Invalid) {
    printf("<%s>", C1_Control__ToString[ctr]);
    i+=1;
  } else {
    if(data[i] == ESC) printf("<ESC>");
    else if(isprint(data[i])) printf("%c",data[i]);
    else printf("\\%02X", (uint8_t)data[i]);
  }
  }
}

bool ansi_find_cmd_end(const char* data, size_t len, size_t* out_len, C1_Control* out_ctrl) {
  if(data[0]!= ESC) return false;

  C1_Control ctr = ansi_match_c1_controls(data,len);
  if(out_ctrl) * out_ctrl = ctr;
  if(ctr == C1_Control_Invalid) return false;

  else if(ctr >= C1_Control_Special_Start && ctr <= C1_Control_Special_Max) {
  if(out_len) *out_len = strlen(C1_Control__InputMap[ctr]);
  return true;
  }

  size_t C1_len = (ctr != C1_Control_Invalid) ? strlen(C1_Control__InputMap[ctr]): 2;
  switch(ctr) {
  case C1_Control_Invalid:
    // printf("<Invalid C1 control: "); ansi_debug(data, C1_len); printf(">");
    if(out_len) *out_len = C1_len;
    return true;
  case C1_Control_CSI: break;
  case C1_Control_VT100_G0:
  case C1_Control_VT100_G1:
  case C1_Control_VT100_G2:
  case C1_Control_VT100_G3: {
    if(out_len) *out_len = C1_len + 1;
  } return true;

  default: {
    printf("<Unhandled C1 control: "); ansi_debug(C1_Control__InputMap[ctr], C1_len ); printf(">");
    if(out_len) *out_len = C1_len;
    return true;
  }
  }


  for(size_t i=C1_len;i<len;i++) {
  if((0x40 <= data[i] && data[i] <= 0x7E)) {
    if(out_len) *out_len = i+1;
    return true;
  };
  }
  return false;
}



typedef enum {
  CSI_SingleCmd_Invalid = -1,

  CSI_SingleCmd_ED,  // Erase display (default 0)
  CSI_SingleCmd_ED0, //  -> from cursor to endof screen
  CSI_SingleCmd_ED1, //  -> from cursor to beging of screen
  CSI_SingleCmd_ED2, //  -> entire visible screen
  CSI_SingleCmd_ED3, //  -> screen and scrollback

  CSI_SingleCmd_EL,  // Erase line (default 0)
  CSI_SingleCmd_EL0, //  -> from cursor to end
  CSI_SingleCmd_EL1, //  -> from cursor to begin
  CSI_SingleCmd_EL2, //  -> entire line

  CSI_SingleCmd_Aux_On, // enables or disable Aux serial port for printer
  CSI_SingleCmd_Aux_Off,

  CSI_SingleCmd_DSR, // device status report: reports the cursor position writing ESC[<row>;<col>R

  CSI_SingleCmd_SCP,  // save cursor position
  CSI_SingleCmd_RCP, // restore cursor position
  
  CSI_SingleCmd_ShowCursor, // shows the cursor
  CSI_SingleCmd_HideCursor, // hides the cursor
  CSI_SingleCmd_FocusEvent_On, //enables focus events: ESC[I and ESC[0
  CSI_SingleCmd_FocusEvent_Off, //disables focus events 
  CSI_SingleCmd_AltScr_On,    //enables alt screen
  CSI_SingleCmd_AltScr_Off,   //disables alt screen
  CSI_SingleCmd_BP_On,   //enables backet paste mode
  CSI_SingleCmd_BP_Off,  //disable backet paste mode
  CSI_SingleCmd_StyleReset,  //resets all styles
  CSI_SingleCmd_SoftReset,   //resets the terminal

  CSI_SingleCmd_Max
} CSI_SingleCmd;


int ansi_str_to_int(const char* str, size_t len, int def) {
  if(len == 0) return def;
  char* buff = calloc(len+1,1);
  memcpy(buff, str, len);
  char* end = NULL; 
  int n = strtol(buff, &end, 10);
  if(end != buff+len) n = def;
  free(buff);
  return n;
}

static bool ansi_int_list_grow(ANSI_IntList* out) {
  size_t new_cap = (out->cap == 0) ? 8 : out->cap * 2;
  int* new_items = realloc(out->items, sizeof(int) * new_cap);
  if(!new_items) { free(out->items); out->items = NULL; out->cap = 0; out->len = 0; return false; }
  out->items = new_items;
  out->cap = new_cap;
  return true;
}

bool ansi_parse_int_list(const char* data, size_t len, ANSI_IntList* out, int def) {
  if(!data || !out) return false;
  
  out->items = NULL;
  out->len = 0;
  out->cap = 0;
  
  int current = 0;
  bool has_digit = false;
  
  for(size_t i = 0; i < len; i++) {
    char ch = data[i];
    
    if(ch >= '0' && ch <= '9') {
      current = current * 10 + (ch - '0');
      has_digit = true;
    } else if(ch == ';' || ch == ':') {
      // Separator - add current value
      if(has_digit) {
        if(out->len + 1 > out->cap && !ansi_int_list_grow(out)) return false;
        out->items[out->len++] = current;
      }
      current = 0;
      has_digit = false;
    } else {
      // Non-digit, non-separator - end of number, add current if exists
      if(has_digit) {
        if(out->len + 1 > out->cap && !ansi_int_list_grow(out)) return false;
        out->items[out->len++] = current;
      }
      current = 0;
      has_digit = false;
    }
  }
  
  // Add final number if exists (without trailing separator)
  if(has_digit) {
    if(out->len + 1 > out->cap && !ansi_int_list_grow(out)) return false;
    out->items[out->len++] = current;
  }
  
  return true;
}

//=============================================================================
// ansi_decode_cmd - Main decoder function
//=============================================================================

static bool ansi_append_invalid(ANSI_Cmd* cmd, const char* data, size_t len) {
  ANSI_Location loc = { .data = data, .len = len };
  if(cmd->invalid.cap == 0 || cmd->invalid.items == NULL) {
    cmd->invalid.cap = 16;
    cmd->invalid.len = 0;
    cmd->invalid.items = realloc(NULL, sizeof(cmd->invalid.items[0]) * cmd->invalid.cap);
  }
  else if(cmd->invalid.len+1>=cmd->invalid.cap) {
    cmd->invalid.cap *= 2;
    cmd->invalid.items = realloc(cmd->invalid.items, sizeof(cmd->invalid.items[0]) * cmd->invalid.cap);
  }

  if(cmd->invalid.items) {
    cmd->invalid.items[cmd->invalid.len++] = loc;
  }

  return cmd->invalid.items;
}

static bool ansi_decode_action(ANSI_Cmd* cmd, const char* params, size_t param_len, char final) {

  ansi_parse_int_list(params, param_len, &cmd->_args, 0);
  int n = (cmd->_args.len > 0) ? cmd->_args.items[0] : 0;

  switch(final) {
    case 'J': // ED - Erase in Display
      cmd->action.clear_screen = true;
      if(n >= 0 && n <= 3) cmd->action.clear_screen_mode = (ANSI_EraseMode)n;
      else cmd->action.clear_screen_mode = ANSI_Erase_ToEnd;
      break;

    case 'K': // EL - Erase in Line
      cmd->action.clear_line = true;
      if(n >= 0 && n <= 2) cmd->action.clear_line_mode = (ANSI_EraseMode)n;
      else cmd->action.clear_line_mode = ANSI_Erase_ToEnd;
      break;

    case 'S': // SU - Scroll Up
      cmd->action.scroll = (n > 0) ? n : 1;
      break;

    case 'T': // SD - Scroll Down
      cmd->action.scroll = -(n > 0 ? n : 1);
      break;

    case 's': // Save cursor (ANSI)
      cmd->action.save_restore_index = n;
      cmd->action.save_restore_cursor = ANSI_State_Set;
      break;

    case 'u': // Restore cursor (ANSI)
      cmd->action.save_restore_index = n;
      cmd->action.save_restore_cursor = ANSI_State_Reset;
      break;

    default:
      return ansi_append_invalid(cmd,params - 1, param_len + 2); // Include final char
  }

  return cmd;
}

static bool ansi_decode_dec_private(ANSI_Cmd* cmd, const char* params, size_t params_len, bool enabled) {
  ANSI_Feature_Cmd* feat = &cmd->feature;

  ANSI_IntList args = {0};
  if(!ansi_parse_int_list(params, params_len, &args, 0)) return false;

  for(size_t i = 0; i < args.len; i++) {
    int mode = args.items[i];
    switch(mode) {
      case 1:    feat->cursor_keys                        = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 2:    feat->us_ascii_vt100                     = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 3:    feat->enable_cols132                     = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 4:    feat->smooth_scroll                      = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 5:    feat->reverse_video                      = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 6:    feat->cursor_relative                    = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 7:    feat->wraparround_mode                   = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 8:    feat->autorepeat_keys                    = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 9:    feat->mouse_tracking_on_press            = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 25:   feat->show_cursor                        = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 47:   feat->enable_alt_screen0                 = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 66:   feat->enable_keypad                      = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 67:   feat->enable_backarrow_as_backspace      = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 69:   feat->enable_margin_mode                 = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1000: feat->mouse_tracking_on_pess_and_release = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1001: feat->mouse_tracking_hilite              = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1002: feat->mouse_tracking_cell_motion         = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1003: feat->mouse_tracking_all_motion          = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1004: feat->enable_focus_events                = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1005: feat->enable_utf8_mouse_mode             = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1006: feat->enable_SRG_mouse_mode              = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1007: feat->enable_alt_scroll_mode             = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1047: feat->enable_alt_screen1                 = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1048: feat->save_cursor                        = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 1049: feat->enable_alt_scree_and_save_cursor   = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      case 2004: feat->enable_bracketed_paste_mode        = enabled ? ANSI_State_Set : ANSI_State_Reset; break;
      default: {
        free(args.items);
        return ansi_append_invalid(cmd, params, params_len);
      }
    }
  }
   
  free(args.items);
  return true;
}

static bool ansi_parser_sgr(ANSI_Cmd* cmd, const char* params, size_t params_len) {
  ANSI_Style_Cmd* style = &cmd->style;
  
  ANSI_IntList args = {0};
  if(!ansi_parse_int_list(params, params_len, &args, 0)) return false;
  
  if(args.len == 0) {
    style->reset = 1;
    return true;
  }
  
  for(size_t i = 0; i < args.len; i++) {
    int code = args.items[i];
    
    // Reset
    if(code == 0) {
      style->reset = 1;
    }
    // Bold
    else if(code == 1) style->bold = ANSI_State_Set;
    else if(code == 22) style->bold = ANSI_State_Reset;
    // Italic
    else if(code == 3) style->italic = ANSI_State_Set;
    else if(code == 23) style->italic = ANSI_State_Reset;
    // Underline
    else if(code == 4) style->underline = ANSI_State_Set;
    else if(code == 24) style->underline = ANSI_State_Reset;
    // Underline color (58)
    else if(code == 58 && args.len > i + 1) {
      // Handle underline color 58 codes
      style->ul_change = 1;
      int c = args.items[i+1];
      // 58;5;n for 256 colors - check first!
      if(c == 5 && args.len > i + 2) {
        c = args.items[i+2];
        if(c < 16) {
          style->ul_color.r = ANSI_ColorPalette[c][0];
          style->ul_color.g = ANSI_ColorPalette[c][1];
          style->ul_color.b = ANSI_ColorPalette[c][2];
        } else if(c < 232) {
          int idx = c - 16;
          style->ul_color.r = (idx / 36) * 51;
          style->ul_color.g = ((idx / 6) % 6) * 51;
          style->ul_color.b = (idx % 6) * 51;
        } else {
          int g = (c - 232) * 10 + 8;
          style->ul_color.r = g;
          style->ul_color.g = g;
          style->ul_color.b = g;
        }
        i += 2;
      }
      // 58;2;r;g;b for RGB underline color
      else if(c == 2 && args.len > i + 4) {
        style->ul_color.r = args.items[i+2];
        style->ul_color.g = args.items[i+3];
        style->ul_color.b = args.items[i+4];
        i += 4;
      }
      // Direct 16-color (58;0 to 58;15)
      else if(c >= 0 && c < 16) {
        style->ul_color.r = ANSI_ColorPalette[c][0];
        style->ul_color.g = ANSI_ColorPalette[c][1];
        style->ul_color.b = ANSI_ColorPalette[c][2];
        i++;
      }
    }
    // Blink
    else if(code == 5) style->blink = ANSI_State_Set;
    else if(code == 25) style->blink = ANSI_State_Reset;
    // Inverse
    else if(code == 7) style->inverse = ANSI_State_Set;
    else if(code == 27) style->inverse = ANSI_State_Reset;
    // Hidden/Invisible
    else if(code == 8) style->invisible = ANSI_State_Set;
    else if(code == 28) style->invisible = ANSI_State_Reset;
    // Strikethrough
    else if(code == 9) style->strikethrough = ANSI_State_Set;
    else if(code == 29) style->strikethrough = ANSI_State_Reset;
    // Foreground color (30-37)
    else if(code >= 30 && code <= 37) {
      style->fg_change = 1;
      int idx = code - 30;
      style->fg_color.r = ANSI_ColorPalette[idx][0];
      style->fg_color.g = ANSI_ColorPalette[idx][1];
      style->fg_color.b = ANSI_ColorPalette[idx][2];
    }
    // Background color (40-47)
    else if(code >= 40 && code <= 47) {
      style->bg_change = 1;
      int idx = code - 40;
      style->bg_color.r = ANSI_ColorPalette[idx][0];
      style->bg_color.g = ANSI_ColorPalette[idx][1];
      style->bg_color.b = ANSI_ColorPalette[idx][2];
    }
    // Bright foreground (90-97)
    else if(code >= 90 && code <= 97) {
      style->fg_change = 1;
      if(code == 90) { style->fg_color.r = 128; style->fg_color.g = 128; style->fg_color.b = 128; }
      else if(code == 91) { style->fg_color.r = 255; style->fg_color.g = 85; style->fg_color.b = 85; }
      else if(code == 92) { style->fg_color.r = 85; style->fg_color.g = 255; style->fg_color.b = 85; }
      else if(code == 93) { style->fg_color.r = 255; style->fg_color.g = 255; style->fg_color.b = 85; }
      else if(code == 94) { style->fg_color.r = 85; style->fg_color.g = 85; style->fg_color.b = 255; }
      else if(code == 95) { style->fg_color.r = 255; style->fg_color.g = 85; style->fg_color.b = 255; }
      else if(code == 96) { style->fg_color.r = 85; style->fg_color.g = 255; style->fg_color.b = 255; }
      else if(code == 97) { style->fg_color.r = 255; style->fg_color.g = 255; style->fg_color.b = 255; }
    }
    // Bright background (100-107)
    else if(code >= 100 && code <= 107) {
      style->bg_change = 1;
      if(code == 100) { style->bg_color.r = 128; style->bg_color.g = 128; style->bg_color.b = 128; }
      else if(code == 101) { style->bg_color.r = 255; style->bg_color.g = 85; style->bg_color.b = 85; }
      else if(code == 102) { style->bg_color.r = 85; style->bg_color.g = 255; style->bg_color.b = 85; }
      else if(code == 103) { style->bg_color.r = 255; style->bg_color.g = 255; style->bg_color.b = 85; }
      else if(code == 104) { style->bg_color.r = 85; style->bg_color.g = 85; style->bg_color.b = 255; }
      else if(code == 105) { style->bg_color.r = 255; style->bg_color.g = 85; style->bg_color.b = 255; }
      else if(code == 106) { style->bg_color.r = 85; style->bg_color.g = 255; style->bg_color.b = 255; }
      else if(code == 107) { style->bg_color.r = 255; style->bg_color.g = 255; style->bg_color.b = 255; }
    }
    // 256 color mode foreground (38;5;n)
    else if(args.len > i + 2 && code == 38 && args.items[i+1] == 5) {
      style->fg_change = 1;
      int c = args.items[i+2];
      if(c < 16) {
        style->fg_color.r = ANSI_ColorPalette[c][0];
        style->fg_color.g = ANSI_ColorPalette[c][1];
        style->fg_color.b = ANSI_ColorPalette[c][2];
      } else if(c < 232) {
        int idx = c - 16;
        style->fg_color.r = (idx / 36) * 51;
        style->fg_color.g = ((idx / 6) % 6) * 51;
        style->fg_color.b = (idx % 6) * 51;
      } else {
        int g = (c - 232) * 10 + 8;
        style->fg_color.r = g;
        style->fg_color.g = g;
        style->fg_color.b = g;
      }
      i += 2;
    }
    // 256 color mode background (48;5;n)
    else if(args.len > i + 2 && code == 48 && args.items[i+1] == 5) {
      style->bg_change = 1;
      int c = args.items[i+2];
      if(c < 16) {
        style->bg_color.r = ANSI_ColorPalette[c][0];
        style->bg_color.g = ANSI_ColorPalette[c][1];
        style->bg_color.b = ANSI_ColorPalette[c][2];
      } else if(c < 232) {
        int idx = c - 16;
        style->bg_color.r = (idx / 36) * 51;
        style->bg_color.g = ((idx / 6) % 6) * 51;
        style->bg_color.b = (idx % 6) * 51;
      } else {
        int g = (c - 232) * 10 + 8;
        style->bg_color.r = g;
        style->bg_color.g = g;
        style->bg_color.b = g;
      }
      i += 2;
    }
    // Underline color (58) - same as foreground
    // 58;n for 16 colors
    else if(code == 58 && args.len > i + 1) {
      style->ul_change = 1;
      int c = args.items[i+1];
      // Direct 16-color (58;0 to 58;15)
      if(c >= 0 && c < 16) {
        style->ul_color.r = ANSI_ColorPalette[c][0];
        style->ul_color.g = ANSI_ColorPalette[c][1];
        style->ul_color.b = ANSI_ColorPalette[c][2];
        i++;
      }
      // 58;5;n for 256 colors
      else if(args.items[i+1] == 5 && args.len > i + 2) {
        c = args.items[i+2];
        if(c < 16) {
          style->ul_color.r = ANSI_ColorPalette[c][0];
          style->ul_color.g = ANSI_ColorPalette[c][1];
          style->ul_color.b = ANSI_ColorPalette[c][2];
        } else if(c < 232) {
          int idx = c - 16;
          style->ul_color.r = (idx / 36) * 51;
          style->ul_color.g = ((idx / 6) % 6) * 51;
          style->ul_color.b = (idx % 6) * 51;
        } else {
          int g = (c - 232) * 10 + 8;
          style->ul_color.r = g;
          style->ul_color.g = g;
          style->ul_color.b = g;
        }
        i += 2;
      }
      // 58;2;r;g;b for RGB underline color
      else if(args.items[i+1] == 2 && args.len > i + 4) {
        style->ul_color.r = args.items[i+2];
        style->ul_color.g = args.items[i+3];
        style->ul_color.b = args.items[i+4];
        i += 4;
      }
    }
    // True color foreground (38;2;r;g;b)
    else if(args.len > i + 4 && code == 38 && args.items[i+1] == 2) {
      style->fg_change = 1;
      style->fg_color.r = args.items[i+2];
      style->fg_color.g = args.items[i+3];
      style->fg_color.b = args.items[i+4];
      i += 4;
    }
    // True color background (48;2;r;g;b)
    else if(args.len > i + 4 && code == 48 && args.items[i+1] == 2) {
      style->bg_change = 1;
      style->bg_color.r = args.items[i+2];
      style->bg_color.g = args.items[i+3];
      style->bg_color.b = args.items[i+4];
      i += 4;
    }
  }
  
  free(args.items);
  return true;
}

static bool ansi_decode_csi(ANSI_Cmd* cmd, const char* data, size_t len) {
  // CSI format: ESC [ Pm... final (len is already exact)

  const char* params = data + 2;  // Skip ESC[
  size_t params_len = len - 2;
  char final = data[len - 1];

  // Check for DEC private mode prefix (?) and handle h/l inside
  if(params_len > 0 && params[0] == '?' ) {
    params++;

    if(final == 'h' && final == 'l') { // enable/disable private flags
      return ansi_append_invalid(cmd, data,len);
    } else if (final == 'n') { //TODO: prviate device status report
      return ansi_append_invalid(cmd, data,len);
    }

    return ansi_decode_dec_private(cmd, params, params_len, final == 'h');
  }

  if(strchr("JKSTsu", final)) {
    return ansi_decode_action(cmd, params, params_len + 1, final);
  }

  switch(final) {
    // Cursor movement - relative (CUU/CUD/CUF/CUB)
    case 'A': /*Cursor Up*/ case 'B': /*Cursor Down*/case 'C': /*Cursor Forward*/ case 'D': /*Cursor Back*/ {
      int n = ansi_str_to_int(params, params_len, 1);
      return ansi_append_invalid(cmd, data, len);
    };

    // Cursor movement - absolute line position
    case 'd': {
      int n = ansi_str_to_int(params, params_len, 1);
      cmd->cursor.move_absolute = 1;
      cmd->cursor.absolute_motion.row = (n > 0) ? n : 1;
      return cmd;
    }

    // Cursor movement - relative line position
    case 'e': {
      int n = ansi_str_to_int(params, params_len, 1);
      cmd->cursor.move_relative = 1;
      cmd->cursor.relative_motion.row = n;
      return cmd;
    }

    // Style (SGR)
    case 'm': return ansi_parser_sgr(cmd, params, params_len);
    // Cursor position (CUP/HVP)
    case 'H': case 'f': return ansi_append_invalid(cmd, data, len);
    // Mode set/reset (DECSET/DECRST)
    case 'h': case 'l': return ansi_append_invalid(cmd, data, len);
    // Scroll region
    case 'r': return ansi_append_invalid(cmd, data, len);
    // Device status report (DSR)
    case 'n': return ansi_append_invalid(cmd, data, len);
    case 'c': return ansi_append_invalid(cmd, data, len);
    default : return ansi_append_invalid(cmd, data, len);
  }
}

bool ansi_decode_cmd(ANSI_Cmd* cmd,const char* data, size_t len) {

  if(len == 0) return ansi_append_invalid(cmd, data, len);
  if(data[0] != ESC) return ansi_append_invalid(cmd, data, len);

  // Detect control type and exact length
  C1_Control ctrl;
  size_t cmd_len;
  if(!ansi_find_cmd_end(data, len, &cmd_len, &ctrl)) {
      return ansi_append_invalid(cmd,data, len);
  }

  // Dispatch based on C1_Control type
  switch(ctrl) {
      case C1_Control_CSI: return ansi_decode_csi(cmd, data, cmd_len);
      case C1_Control_OSC: {
          return ansi_append_invalid(cmd,data,len);
      }

      case C1_Control_ST: {
          return ansi_append_invalid(cmd,data,len);
      }

      // Simple 2-byte controls
      case C1_Control_IND:
      case C1_Control_NEL:
      case C1_Control_RI:
      case C1_Control_SS2:
      case C1_Control_SS3:
      case C1_Control_DCS:
      case C1_Control_SPA:
      case C1_Control_EPA:
      case C1_Control_SOS:
      case C1_Control_RTI:
      case C1_Control_APC: {
      }

      // Special VT100 sequences
      case C1_Control_Special_DECSC: {
      }

      case C1_Control_Special_DECRC: {
      }

      case C1_Control_Special_RIS: {
      }

      case C1_Control_Special_ML: {
      }

      case C1_Control_Special_MU: {
      }

      // Character set designation
      case C1_Control_VT100_G0:
      case C1_Control_VT100_G1:
      case C1_Control_VT100_G2:
      case C1_Control_VT100_G3: {
      }

      // Other VT100 special sequences
      case C1_Control_Special_DECPAM:
      case C1_Control_Special_DECPNM:
      case C1_Control_Special_CTLLCS:
      case C1_Control_Special_LS2:
      case C1_Control_Special_LS3:
      case C1_Control_Special_LS3R:
      case C1_Control_Special_LS2R:
      case C1_Control_Special_LS1R:
      case C1_Control_VT100_SP:
      case C1_Control_VT100_DEC:
      case C1_Control_VT100_SEL: {
      }

      default:
          return ansi_append_invalid(cmd, data, cmd_len);
  }
}

void ansi_debug_cmd(const ANSI_Cmd* cmd) {
  if(!cmd) { printf("(null)\n"); return; }
  
  printf("{ ");
  
  // Print invalid commands
  if(cmd->invalid.len > 0) {
    printf("invalid.len=%zu ", cmd->invalid.len);
  }
  
  // Print action commands
  if(cmd->action.clear_screen) {
    printf("action.clear_screen=%d ", cmd->action.clear_screen_mode);
  }
  if(cmd->action.clear_line) {
    printf("action.clear_line=%d ", cmd->action.clear_line_mode);
  }
  if(cmd->action.scroll != 0) {
    printf("action.scroll=%d ", cmd->action.scroll);
  }
  if(cmd->action.save_restore_cursor != 0) {
    printf("action.save_restore_cursor=%d ", cmd->action.save_restore_cursor);
  }
  if(cmd->action.soft_reset) {
    printf("action.soft_reset=%d ", cmd->action.soft_reset);
  }
  
  // Print style commands
if(cmd->style.reset) printf("style.reset=1 ");
  if(cmd->style.bold) printf("style.bold=%d ", cmd->style.bold);
  if(cmd->style.italic) printf("style.italic=%d ", cmd->style.italic);
  if(cmd->style.underline) printf("style.underline=%d ", cmd->style.underline);
  if(cmd->style.fg_change) {
    printf("style.fg=#%02X%02X%02X", cmd->style.fg_color.r, cmd->style.fg_color.g, cmd->style.fg_color.b);
    printf("\033[48;2;%d;%d;%dm  \033[0m", cmd->style.fg_color.r, cmd->style.fg_color.g, cmd->style.fg_color.b);
  }
  if(cmd->style.bg_change) {
    printf(" style.bg=#%02X%02X%02X", cmd->style.bg_color.r, cmd->style.bg_color.g, cmd->style.bg_color.b);
    printf("\033[48;2;%d;%d;%dm  \033[0m", cmd->style.bg_color.r, cmd->style.bg_color.g, cmd->style.bg_color.b);
  }
  if(cmd->style.ul_change) {
    printf("style.ul=#%02X%02X%02X", cmd->style.ul_color.r, cmd->style.ul_color.g, cmd->style.ul_color.b);
    printf("\e[48;2;%d;%d;%dm  \e[0m", cmd->style.ul_color.r, cmd->style.ul_color.g, cmd->style.ul_color.b);
  }
  
  // Print cursor commands
  if(cmd->cursor.move_absolute) {
    printf("cursor.move_absolute=%d ", cmd->cursor.move_absolute);
  }
  if(cmd->cursor.move_relative) {
    printf("cursor.move_relative=%d ", cmd->cursor.move_relative);
  }
  if(cmd->cursor.visible) printf("cursor.visible=%d ", cmd->cursor.visible);
  
  // Print window commands
  if(cmd->window.window_resize) printf("window.window_resize=%d ", cmd->window.window_resize);
  if(cmd->window.scroll_region) printf("window.scroll_region=%d ", cmd->window.scroll_region);
  
  // Print feature commands
  if(cmd->feature.cursor_keys)         printf("feature.cursor_keys=%d ", cmd->feature.cursor_keys);
  if(cmd->feature.us_ascii_vt100)  printf("feature.us_ascii_vt100=%d ", cmd->feature.us_ascii_vt100);
  if(cmd->feature.enable_cols132)  printf("feature.enable_cols132=%d ", cmd->feature.enable_cols132);
  if(cmd->feature.smooth_scroll)  printf("feature.smooth_scroll=%d ", cmd->feature.smooth_scroll);
  if(cmd->feature.reverse_video)  printf("feature.reverse_video=%d ", cmd->feature.reverse_video);
  if(cmd->feature.cursor_relative)  printf("feature.cursor_relative=%d ", cmd->feature.cursor_relative);
  if(cmd->feature.wraparround_mode) printf("feature.wraparround_mode=%d ", cmd->feature.wraparround_mode);
  if(cmd->feature.autorepeat_keys) printf("feature.autorepeat_keys=%d ", cmd->feature.autorepeat_keys);
  if(cmd->feature.mouse_tracking_on_press) printf("feature.mouse_tracking_on_press=%d ", cmd->feature.mouse_tracking_on_press);
  if(cmd->feature.show_cursor)          printf("feature.show_cursor=%d ", cmd->feature.show_cursor);
  if(cmd->feature.enable_alt_screen0)   printf("feature.enable_alt_screen0=%d ", cmd->feature.enable_alt_screen0);
  if(cmd->feature.enable_keypad) printf("feature.enable_keypad=%d ", cmd->feature.enable_keypad);
  if(cmd->feature.enable_backarrow_as_backspace) printf("feature.enable_backarrow_as_backspace=%d ", cmd->feature.enable_backarrow_as_backspace);
  if(cmd->feature.enable_margin_mode) printf("feature.enable_margin_mode=%d ", cmd->feature.enable_margin_mode);
  if(cmd->feature.mouse_tracking_on_pess_and_release) printf("feature.mouse_tracking_on_pess_and_release=%d ", cmd->feature.mouse_tracking_on_pess_and_release);
  if(cmd->feature.mouse_tracking_hilite) printf("feature.mouse_tracking_hilite=%d ", cmd->feature.mouse_tracking_hilite);
  if(cmd->feature.mouse_tracking_cell_motion) printf("feature.mouse_tracking_cell_motion=%d ", cmd->feature.mouse_tracking_cell_motion);
  if(cmd->feature.mouse_tracking_all_motion) printf("feature.mouse_tracking_all_motion=%d ", cmd->feature.mouse_tracking_all_motion);
  if(cmd->feature.enable_focus_events) printf("feature.enable_focus_events=%d ", cmd->feature.enable_focus_events);
  if(cmd->feature.enable_utf8_mouse_mode) printf("feature.enable_utf8_mouse_mode=%d ", cmd->feature.enable_utf8_mouse_mode);
  if(cmd->feature.enable_SRG_mouse_mode) printf("feature.enable_SRG_mouse_mode=%d ", cmd->feature.enable_SRG_mouse_mode);
  if(cmd->feature.enable_alt_scroll_mode) printf("feature.enable_alt_scroll_mode=%d ", cmd->feature.enable_alt_scroll_mode);
  if(cmd->feature.enable_alt_screen1)    printf("feature.enable_alt_screen1=%d ", cmd->feature.enable_alt_screen1);
  if(cmd->feature.save_cursor)          printf("feature.save_cursor=%d ", cmd->feature.save_cursor);
  if(cmd->feature.enable_alt_scree_and_save_cursor) printf("feature.enable_alt_scree_and_save_cursor=%d ", cmd->feature.enable_alt_scree_and_save_cursor);
  if(cmd->feature.enable_bracketed_paste_mode) printf("feature.enable_bracketed_paste_mode=%d ", cmd->feature.enable_bracketed_paste_mode);
  
  // Print dsr commands
  if(cmd->dsr.status) printf("dsr.status=%d ", cmd->dsr.status);
  if(cmd->dsr.cursor) printf("dsr.cursor=%d ", cmd->dsr.cursor);
  
  // Print fill commands
  if(cmd->fill.fill_rectangle) printf("fill.fill_rectangle=(%d,%d,%d,%d) ", 
    cmd->fill.rectangle.top, cmd->fill.rectangle.left, cmd->fill.rectangle.bottom, cmd->fill.rectangle.right);
  if(cmd->fill.erase_rectangle) printf("fill.erase_rectangle=(%d,%d,%d,%d) ", 
    cmd->fill.rectangle.top, cmd->fill.rectangle.left, cmd->fill.rectangle.bottom, cmd->fill.rectangle.right);
  if(cmd->fill.copy_rectangle) printf("fill.copy_rectangle=(%d,%d,%d,%d) ", 
    cmd->fill.rectangle.top, cmd->fill.rectangle.left, cmd->fill.rectangle.bottom, cmd->fill.rectangle.right);
  
  printf("}");
}
