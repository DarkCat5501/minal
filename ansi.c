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

  for(int i=0;i<VT100_CharSet__InputMap;i++) {
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
  
  for(size_t i = 0; i <= len; i++) {
    char ch = (i < len) ? data[i] : ';'; // Treat end as separator
    
    if(ch >= '0' && ch <= '9') {
      current = current * 10 + (ch - '0');
      has_digit = true;
    } else if(ch == ';' || ch == ':') {
      // Separator - add current value
      if(out->len + 1 > out->cap && !ansi_int_list_grow(out)) return false;
      out->items[out->len++] = has_digit ? current : def;
      current = 0;
      has_digit = false;
    } else {
      // Non-digit, non-separator - treat as separator with default
      if(has_digit) {
        if(out->len + 1 > out->cap && !ansi_int_list_grow(out)) return false;
        out->items[out->len++] = current;
      }
      current = 0;
      has_digit = false;
    }
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
  // Parse parameter number and call ansi_split_args to handle multiple params
  ansi_split_args(params - 1, params_len + 1, NULL, 0); // Reset state

  ANSI_Feature_Cmd* feat = &cmd->feature;

  const char* arg;
  size_t arg_len;
  while(ansi_split_args(params, params_len, &arg, &arg_len)) {
      int mode = ansi_str_to_int(arg, arg_len, 0);
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
          return ansi_append_invalid(cmd, params, params_len);
        } break;
      }
  }
  
  return true;
}

static bool ansi_decode_csi(ANSI_Cmd* cmd, const char* data, size_t len) {
  // CSI format: ESC [ Pm... final (len is already exact)
  // if(len < 2 || data[0] != '[') return ansi_append_invalid(cmd,data, len);

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

    // Style (SGR) //TODO: parse ansi styles
    case 'm': return ansi_append_invalid(cmd, data, len);
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
