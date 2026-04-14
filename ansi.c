#include "ansi.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ESC '\033'

typedef enum {
  C1_Control_Invalid = -1,
  C1_Control_IND,
  C1_Control_NEL,
  C1_Control_HTS,
  C1_Control_RI ,
  C1_Control_SS2,
  C1_Control_SS3,
  C1_Control_DCS,
  C1_Control_SPA,
  C1_Control_EPA,
  C1_Control_SOS,
  C1_Control_RTI,
  C1_Control_CSI,
  C1_Control_ST ,
  C1_Control_OSC,
  C1_Control_APC,
  C1_Control_VT100_SP,
  C1_Control_VT100_DEC,
  C1_Control_VT100_SEL,
  C1_Control_VT100_G0,
  C1_Control_VT100_G1,
  C1_Control_VT100_G2,
  C1_Control_VT100_G3,
  C1_Control_Max = C1_Control_VT100_G3
} C1_Control;

typedef enum {
  VT100_Special_Invalid = -1,
  VT100_Special_DECSC,  // Save Cursor (DECSC)
  VT100_Special_DECRC,  // Restore Cursor (DECRC)
  VT100_Special_DECPAM, // Application Keypad (DECPAM)
  VT100_Special_DECPNM, // Normal Keypad (DECPNM)
  VT100_Special_CTLLCS, // Cursor to lower left corner of screen (if enabled by the hpLowerleftBugCompat resource).
  VT100_Special_RIS,    // Full Reset (RIS)
  VT100_Special_ML,     // Memory Lock (per HP terminals). Locks memory above the cursor.
  VT100_Special_MU,     // Memory Unlock (per HP terminals)
  VT100_Special_LS2,    // Invoke the G2 Character Set as GL (LS2).
  VT100_Special_LS3,    // Invoke the G3 Character Set as GL (LS3).
  VT100_Special_LS3R,   // Invoke the G3 Character Set as GR (LS3R).
  VT100_Special_LS2R,   // Invoke the G2 Character Set as GR (LS2R).
  VT100_Special_LS1R,   // Invoke the G1 Character Set as GR (LS1R).
  VT100_Special_Max = VT100_Special_LS1R
} VT100_Special;


typedef enum {
  VT100_CharSet_Invalid = -1,
  VT100_CharSet_Special_LineDrawing, //0 → DEC Special Character and Line Drawing Set
  VT100_CharSet_UnitedKingdom,       //A → United Kingdom (UK)
  VT100_CharSet_UnitedStates,//B → United States (USASCII)
  VT100_CharSet_Dutch,//4 → Dutch
  VT100_CharSet_Finnish,//C or 5 → Finnish
  VT100_CharSet_French,//R → French
  VT100_CharSet_FrenchCanadian,//Q → French Canadian
  VT100_CharSet_German,//K → German
  VT100_CharSet_Italian,//Y → Italian
  VT100_CharSet_Norwegian,//E or 6 → Norwegian/Danish
  VT100_CharSet_Spanish,//Z → Spanish
  VT100_CharSet_Swidsh,//H or 7 → Swedish
  VT100_CharSet_Swiss,//= → Swiss
  VT100_CharSet_Max = VT100_CharSet_Swiss
} VT100_CharSet;

static char* C1_Controls_Map[] = {
  [C1_Control_IND] =  "\033D",
  [C1_Control_NEL] =  "\033E",
  [C1_Control_HTS] =  "\033H",
  [C1_Control_RI ] =  "\033M",
  [C1_Control_SS2] =  "\033N",
  [C1_Control_SS3] =  "\033O",
  [C1_Control_DCS] =  "\033P",
  [C1_Control_SPA] =  "\033V",
  [C1_Control_EPA] =  "\033W",
  [C1_Control_SOS] =  "\033X",
  [C1_Control_RTI] =  "\033Z",
  [C1_Control_CSI] =  "\033[",
  [C1_Control_ST ] =  "\033\\",
  [C1_Control_OSC] =  "\033]",
  [C1_Control_APC] =  "\033_",
  [C1_Control_VT100_SP]   = "\033 ",
  [C1_Control_VT100_DEC]  = "\033#",
  [C1_Control_VT100_SEL]  = "\033%",
  [C1_Control_VT100_G0]   = "\033(",
  [C1_Control_VT100_G1]   = "\033)",
  [C1_Control_VT100_G2]   = "\033*",
  [C1_Control_VT100_G3]   = "\033+",
};

static char* C1_Controls_ToString[] = {
  [C1_Control_IND] =  "IND",
  [C1_Control_NEL] =  "NEL",
  [C1_Control_HTS] =  "HTS",
  [C1_Control_RI ] =  "RI",
  [C1_Control_SS2] =  "SS2",
  [C1_Control_SS3] =  "SS3",
  [C1_Control_DCS] =  "DCS",
  [C1_Control_SPA] =  "SPA",
  [C1_Control_EPA] =  "EPA",
  [C1_Control_SOS] =  "SOS",
  [C1_Control_RTI] =  "RTI",
  [C1_Control_CSI] =  "CSI",
  [C1_Control_ST ] =  "ST",
  [C1_Control_OSC] =  "OSC",
  [C1_Control_APC] =  "APC",
  [C1_Control_VT100_SP]   = "VT100SP",
  [C1_Control_VT100_DEC]  = "VT100DEC",
  [C1_Control_VT100_SEL]  = "VT100SEL",
  [C1_Control_VT100_G0]   = "VT100G0",
  [C1_Control_VT100_G1]   = "VT100G1",
  [C1_Control_VT100_G2]   = "VT100G2",
  [C1_Control_VT100_G3]   = "VT100G3",
};

static char* VT100_CharSets_Map[] = {
  [VT100_CharSet_Special_LineDrawing] =   "0",
  [VT100_CharSet_UnitedKingdom] =         "A",
  [VT100_CharSet_UnitedStates] =          "B",
  [VT100_CharSet_Dutch] =                 "4",
  [VT100_CharSet_Finnish] =               "C5",
  [VT100_CharSet_French] =                "R",
  [VT100_CharSet_FrenchCanadian] =        "Q",
  [VT100_CharSet_German] =                "K",
  [VT100_CharSet_Italian] =               "Y",
  [VT100_CharSet_Norwegian] =             "E6",
  [VT100_CharSet_Spanish] =               "Z",
  [VT100_CharSet_Swidsh] =                "H7",
  [VT100_CharSet_Swiss] =                 "=",
};

static char* VT100_Specials_Map[] = {
 [VT100_Special_DECSC] = "\0337",
 [VT100_Special_DECRC] = "\0338",
 [VT100_Special_DECPAM]= "\033=",
 [VT100_Special_DECPNM]= "\033>",
 [VT100_Special_CTLLCS]= "\033F",
 [VT100_Special_RIS]   = "\033c",
 [VT100_Special_ML]    = "\033l",
 [VT100_Special_MU]    = "\033m",
 [VT100_Special_LS2]   = "\033n",
 [VT100_Special_LS3]   = "\033o",
 [VT100_Special_LS3R]  = "\033|",
 [VT100_Special_LS2R]  = "\033}",
 [VT100_Special_LS1R]  = "\033~",
};


static C1_Control ansi_match_c1_controls(const char* data, size_t len) {
  static_assert(C1_Control_Max == C1_Control_VT100_G3, "Unhandled C1 Control");
  if(len < 2) goto invalid;

  for(int i=0;i<C1_Control_Max;i++) {
    if(strncmp(C1_Controls_Map[i],data,2)==0) return (C1_Control)i;
  }
invalid:
  return C1_Control_Invalid;
}

static VT100_Special ansi_match_vt100_spacials(const char* data, size_t len) {
  static_assert(VT100_Special_Max == VT100_Special_LS1R, "Unhandle VT100 Special");
  if(len < 2) goto invalid;

  for(int i=0;i<VT100_Special_Max;i++) {
    if(strncmp(VT100_Specials_Map[i],data,2)==0) return (VT100_Special)i;
  }
invalid:
  return VT100_Special_Invalid;
}

static VT100_CharSet ansi_match_vt100_charset(const char* data, size_t len) {
  static_assert(VT100_CharSet_Max == VT100_CharSet_Swiss, "Unhandle VT100 CharSet");
  if(len < 2) goto invalid;

  for(int i=0;i<VT100_CharSet_Max;i++) {
    if(strncmp(VT100_CharSets_Map[i],data,2)==0) return (VT100_CharSet)i;
  }
invalid:
  return VT100_CharSet_Invalid;
}

void ansi_debug(const char* data, size_t len) {
  for(size_t i=0;i<len;i++) {
    C1_Control ctr = ansi_match_c1_controls(data+i,len-i);
    if(ctr != C1_Control_Invalid) {
      printf("<%s>", C1_Controls_ToString[ctr]);
      i+=1;
    } else {
      if(data[i] == ESC) printf("<ESC>");
      else if(isprint(data[i])) printf("%c",data[i]);
      else printf("\\%02X", (uint8_t)data[i]);
    }
  }
}

bool ansi_find_cmd_end(const char* data, size_t len, size_t* out_len) {

  if(data[0]!= ESC) return false;

  C1_Control ctr = ansi_match_c1_controls(data,len);
  if(ctr != C1_Control_Invalid) goto handle_c1;
  
  VT100_Special spc = ansi_match_vt100_spacials(data,len);
  if(spc != VT100_Special_Invalid) goto handle_vt100_special;

  return false;

handle_vt100_special:
  // printf("S");
  if(out_len) *out_len = strlen(VT100_Specials_Map[spc]);
  return true;

handle_c1:
  size_t C1_len = (ctr != C1_Control_Invalid) ? strlen(C1_Controls_Map[ctr]): 2;
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
      printf("<Unhandled C1 control: "); ansi_debug(C1_Controls_Map[ctr], C1_len ); printf(">");
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

bool ansi_split_int_args(const char* data, size_t len, int* out_n, int def) {
  const char* out; size_t out_len;
  if(!ansi_split_args(data,len,&out,&out_len)) {
    *out_n = def;
    return false;
  }
  *out_n = ansi_str_to_int(out,out_len,def);
  return true;
}

static const char* current_data = NULL;
static size_t current_index     = 0;

bool ansi_split_args(const char* data, size_t len, const char** out, size_t* out_len) {

  if(data) {
    current_data = data;
    current_index = 0;
  }

  data = current_data;

  if(data != current_data) {
    current_index = 0;
  }

  if(current_index > len) return false;

  for(size_t i=current_index;i<len;i++) {
    char ch = data[i];

    if(ch == ';' || ch == ':') {
      *out = data + current_index;
      *out_len = i - current_index;
      current_index = i+1;
      return true;
    }
  }

  *out = data + current_index;
  *out_len = len - current_index;
  current_index = len;
  
  return true;
}


// void minal_parse_ansi_osc(Minal* m, StringView* bytes)
// {
//     uint8_t b = sv_first(bytes);
//
//     int argv[10];
//     int argc = 0;
//     if (isdigit(b)) {
//         minal_parse_ansi_args(m, bytes, &argc, argv);
//         assert(argc > 0);
//         b = argv[0];
//     }
//
//     switch (b) {
//         case STP_ICON_NAME_WINDOW_TITLE: {
//             printf("TODO: STP_ICON_NAME_WINDOW_TITLE\n");
//         }; break;
//
//         case STP_ICON_NAME: {
//             printf("TODO: STP_ICON_NAME\n");
//         }; break;
//
//         case STP_WINDOW_TITLE: {
//             printf("TODO: STP_WINDOW_TITLE\n");
//         }; break;
//
//         case STP_X_PROPERTY: {
//             printf("TODO: STP_X_PROPERTY\n");
//         }; break;
//
//         case STP_COLOR_NUMBER: {
//             printf("TODO: STP_COLOR_NUMBER\n");
//         }; break;
//
//         case STP_SPECIAL_COLOR_NUMBER: {
//             printf("TODO: STP_SPECIAL_COLOR_NUMBER\n");
//         }; break;
//
//         case STP_TOGGLE_SPECIAL_CLRNUM: {
//             printf("TODO: STP_TOGGLE_SPECIAL_CLRNUM\n");
//         }; break;
//
//         case STP_VT100_FG_COLOR: {
//             printf("TODO: STP_VT100_FG_COLOR\n");
//         }; break;
//
//         case STP_VT100_BG_COLOR: {
//             printf("TODO: STP_VT100_BG_COLOR\n");
//         }; break;
//
//         case STP_TEXT_CURSOR_COLOR: {
//             printf("TODO: STP_TEXT_CURSOR_COLOR\n");
//         }; break;
//
//         case STP_POINTER_FG_COLOR: {
//             printf("TODO: STP_POINTER_FG_COLOR\n");
//         }; break;
//
//         case STP_POINTER_BG_COLOR: {
//             printf("TODO: STP_POINTER_BG_COLOR\n");
//         }; break;
//
//         case STP_TEKTRONIX_FG_COLOR: {
//             printf("TODO: STP_TEKTRONIX_FG_COLOR\n");
//         }; break;
//
//         case STP_TEKTRONIX_BG_COLOR: {
//             printf("TODO: STP_TEKTRONIX_BG_COLOR\n");
//         }; break;
//
//         case STP_HIGHLIGHT_BG_COLOR: {
//             printf("TODO: STP_HIGHLIGHT_BG_COLOR\n");
//         }; break;
//
//         case STP_TEKTRONIX_CURSOR_COLOR: {
//             printf("TODO: STP_TEKTRONIX_CURSOR_COLOR\n");
//         }; break;
//
//         case STP_HIGHLIGHT_FG_COLOR: {
//             printf("TODO: STP_HIGHLIGHT_FG_COLOR\n");
//         }; break;
//
//         case STP_POINTER_CURSOR_SHAPE: {
//             printf("TODO: STP_POINTER_CURSOR_SHAPE\n");
//         }; break;
//
//         case STP_CHANGE_LOG_FILE: {
//             printf("TODO: STP_CHANGE_LOG_FILE\n");
//         }; break;
//
//         case STP_SET_FONT: {
//             printf("TODO: STP_SET_FONT\n");
//         }; break;
//
//         case STP_FOR_EMACS: {
//             printf("TODO: STP_FOR_EMACS\n");
//         }; break;
//
//         case STP_MANIP_SELECTION_DATA: {
//             printf("TODO: STP_MANIP_SELECTION_DATA\n");
//         }; break;
//
//         case STP_XTERM_QUERY_ALLOWED: {
//             printf("TODO: STP_XTERM_QUERY_ALLOWED\n");
//         }; break;
//
//         case STP_XTERM_QUERY_DISALLOWED: {
//             printf("TODO: STP_XTERM_QUERY_DISALLOWED\n");
//         }; break;
//
//         case STP_XTERM_QUERY_ALLOWABLE: {
//             printf("TODO: STP_XTERM_QUERY_ALLOWABLE\n");
//         }; break;
//
//         case STP_RESET_COLOR: {
//             printf("TODO: STP_RESET_COLOR\n");
//         }; break;
//
//         case STP_RESET_SPECIAL_COLOR: {
//             printf("TODO: STP_RESET_SPECIAL_COLOR\n");
//         }; break;
//
//         case STP_TOGGLE_SPECIAL_COLOR: {
//             printf("TODO: STP_TOGGLE_SPECIAL_COLOR\n");
//         }; break;
//
//         case STP_RESET_VT100_TXTFGCLR: {
//             printf("TODO: STP_RESET_VT100_TXTFGCLR\n");
//         }; break;
//
//         case STP_RESET_VT100_TXTBGCLR: {
//             printf("TODO: STP_RESET_VT100_TXTBGCLR\n");
//         }; break;
//
//         case STP_RESET_TEXT_CURSOR_COLOR: {
//             printf("TODO: STP_RESET_TEXT_CURSOR_COLOR\n");
//         }; break;
//
//         case STP_RESET_POINTER_FG_COLOR: {
//             printf("TODO: STP_RESET_POINTER_FG_COLOR\n");
//         }; break;
//
//         case STP_RESET_POINTER_BG_COLOR: {
//             printf("TODO: STP_RESET_POINTER_BG_COLOR\n");
//         }; break;
//
//         case STP_RESET_TKTX_FG_COLOR: {
//             printf("TODO: STP_RESET_TKTX_FG_COLOR\n");
//         }; break;
//
//         case STP_RESET_TKTX_BG_COLOR: {
//             printf("TODO: STP_RESET_TKTX_BG_COLOR\n");
//         }; break;
//
//         case STP_RESET_HIGHLIGHT_COLOR: {
//             printf("TODO: STP_RESET_HIGHLIGHT_COLOR\n");
//         }; break;
//
//         case STP_RESET_TKTX_CURSOR_COLOR: {
//             printf("TODO: STP_RESET_TKTX_CURSOR_COLOR\n");
//         }; break;
//
//         case STP_RESET_HIGHLIGHT_FGCOLOR: {
//             printf("TODO: STP_RESET_HIGHLIGHT_FGCOLOR\n");
//         }; break;
//
//         case STP_SET_ICON_TO_FILE: {
//             printf("TODO: STP_SET_ICON_TO_FILE\n");
//         }; break;
//
//         case STP_SET_WINDOW_TITLE: {
//             printf("TODO: STP_SET_WINDOW_TITLE\n");
//         }; break;
//
//         case STP_SET_ICON_LABEL: {
//             printf("TODO: STP_SET_ICON_LABEL\n");
//         }; break;
//
//         case STP_TODO_FIGURE_THIS_OUT: {
//             printf("TODO: ESC OSC 7; <t> ST => WHAT THE F IS ?THIS MA?N?????\n");
//         }; break;
//
//
//         default: printf("UNKNOWN OSC COMMAND: %08b | %02X (%c)\n", b, b, b); break;
//     }
//
//     b = sv_chop_left(bytes);
//     while (b != STP_TERMINATOR_1 && b != STP_TERMINATOR_2 && bytes->len) {
//         b = sv_chop_left(bytes);
//     }
// }
//
//
// void minal_parse_ansi_csi(Minal* m, StringView* bytes)
// {
//     uint8_t b = sv_first(bytes);
//
//     // the <ops> in [!, =, >, ?, u, s] comes before the arguments
//     switch (b) {
//
//         case CSI_BANG_PREFIX: {
//             printf("TODO: CSI !\n");
//         }; break;
//
//         case CSI_EQUALS_PREFIX: {
//             printf("TODO: CSI = \n");
//         }; break;
//
//         case CSI_GT_PREFIX: {
//             sv_chop_left(bytes);
//             int argv[10] = {-1};
//             int argc     = 0;
//             minal_parse_ansi_args(m, bytes, &argc, argv);
//
//             b = sv_chop_left(bytes);
//             switch (b) {
//                 case 'T': {
//                     printf("TODO: CSI > '%c'\n", b);
//                 }; break;
//                 case 'c': {
//                     printf("TODO: CSI > '%c'\n", b);
//                 }; break; 
//                 case 'f': {
//                     printf("TODO CSI > '%c'\n", b);
//                 }; break;
//                 case 'm': {
//                     printf("TODO CSI > '%c'\n", b);
//                 }; break;
//                 case 'n': {
//                     printf("TODO CSI > '%c'\n", b);
//                 }; break;
//                 case 'p': {
//                     printf("TODO CSI > '%c'\n", b);
//                 }; break;
//                 case 'q': {
//                     printf("TODO CSI > '%c'\n", b);
//                 }; break;
//                 case 's': {
//                     int opt = 0;
//                     if (argc > 0) {
//                         opt = argv[0];
//                     }
//
//                     if (opt != 0 && opt != 1) {
//                         printf("ERROR: invalid XTSHIFTESCAPE argument: expected '0' or '1', got '%d'\n", opt);
//                         return;
//                     }
//
//                     switch (opt) {
//                         case 0: 
//                         case 1: {
//                             printf("TODO: CSI XTSHITESCAPE not implemented\n");
//                         }; break;
//                     }
//
//                 }; break;
//                 case 't': {
//                     printf("TODO CSI > '%c'\n", b);
//                 }; break;
//                 default: printf("UNKNOWN subcommando for CSI_GT_PREFIX: %08b | %02X (%c)\n", b, b, b);
//             }
//             return;
//         };
//
//         case CSI_QUESTION_MARK_PREFIX: {
//             sv_chop_left(bytes);
//
//             int opt = -1;
//             if (isdigit(sv_first(bytes))) {
//                 opt = sv_parse_int(bytes);
//             }
//
//             b = sv_chop_left(bytes);
//
//             switch (opt) {
//                 case DECSET_APPLICATION_CURSOR_KEYS:    { printf("TODO: DECSET_APPLICATION_CURSOR_KEYS\n"); } break;    
//                 case DECSET_DESIGNATE_USASCII_G0_TO_G3: { printf("TODO: DECSET_DESIGNATE_USASCII_G0_TO_G3\n"); } break; 
//                 case DECSET_COLUMN_MODE:                { printf("TODO: DECSET_COLUMN_MODE\n"); } break;                
//                 case DECSET_SMOOTH_SCROLL:              { printf("TODO: DECSET_SMOOTH_SCROLL\n"); } break;              
//                 case DECSET_REVERSE_VIDEO:              { printf("TODO: DECSET_REVERSE_VIDEO\n"); } break;              
//                 case DECSET_ORIGIN_MODE:                { printf("TODO: DECSET_ORIGIN_MODE\n"); } break;                
//                 case DECSET_AUTO_WRAP_MODE:             { m->autowrap = b == 'h'; } break;             
//                 case DECSET_AUTO_REPEAT_KEYS:           { printf("TODO: DECSET_AUTO_REPEAT_KEYS\n"); } break;           
//                 case DECSET_SEND_MOUSE_X_Y_ON_BUTPRESS: { printf("TODO: DECSET_SEND_MOUSE_X_Y_ON_BUTPRESS\n"); } break; 
//                 case DECSET_SHOW_TOOLBAR:               { printf("TODO: DECSET_SHOW_TOOLBAR\n"); } break;               
//                 case DECSET_START_BLINKING_CURSOR:      { printf("TODO: DECSET_START_BLINKING_CURSOR\n"); } break;      
//                 case DECSET_START_BLINKING_CURSOR_2:    { printf("TODO: DECSET_START_BLINKING_CURSOR_2\n"); } break;    
//                 case DECSET_XOR_BLINKING_CURSOR:        { printf("TODO: DECSET_XOR_BLINKING_CURSOR\n"); } break;        
//                 case DECSET_PRINT_FORM_FEED:            { printf("TODO: DECSET_PRINT_FORM_FEED\n"); } break;            
//                 case DECSET_SET_PRINT_EXT_FULLSCREEN:   { printf("TODO: DECSET_SET_PRINT_EXT_FULLSCREEN\n"); } break;   
//                 case DECSET_SHOW_CURSOR:                { printf("TODO: DECSET_SHOW_CURSOR\n"); } break;                
//                 case DECSET_SHOW_SCROLLBAR:             { printf("TODO: DECSET_SHOW_SCROLLBAR\n"); } break;             
//                 case DECSET_ENB_FONT_SHIFTING:          { printf("TODO: DECSET_ENB_FONT_SHIFTING\n"); } break;          
//                 case DECSET_ENTER_TEKTRONIX_MODE:       { printf("TODO: DECSET_ENTER_TEKTRONIX_MODE\n"); } break;       
//                 case DECSET_132_MODE:                   { printf("TODO: DECSET_132_MODE\n"); } break;                   
//                 case DECSET_MORE_FIX:                   { printf("TODO: DECSET_MORE_FIX\n"); } break;                   
//                 case DECSET_ENB_NAT_REPLACE_CHSET:      { printf("TODO: DECSET_ENB_NAT_REPLACE_CHSET\n"); } break;      
//                 case DECSET_ENB_GRAP_EXP_PRINT_MODE:    { printf("TODO: DECSET_ENB_GRAP_EXP_PRINT_MODE\n"); } break;    
//                 case DECSET_TURN_ON_MARGIN_BELL:        { printf("TODO: DECSET_TURN_ON_MARGIN_BELL\n"); } break;        
//                 // case DECSET_ENB_GRAP_PRINT_CLR_MODE:    { printf("TODO: DECSET_ENB_GRAP_PRINT_CLR_MODE\n"); } break;    
//                 case DECSET_REVERSE_WRAP_MODE:          { printf("TODO: DECSET_REVERSE_WRAP_MODE\n"); } break;          
//                 // case DECSET_ENB_GRAP_PRINT_CLR_SYNTAX:  { printf("TODO: DECSET_ENB_GRAP_PRINT_CLR_SYNTAX\n"); } break;  
//                 case DECSET_START_LOGGING:              { printf("TODO: DECSET_START_LOGGING\n"); } break;              
//                 // case DECSET_GRAP_PRINT_BG_MODE:         { printf("TODO: DECSET_GRAP_PRINT_BG_MODE\n"); } break;         
//                 case DECSET_USE_ALTERNATE_SCREEN_BUF:   { printf("TODO: DECSET_USE_ALTERNATE_SCREEN_BUF\n"); } break;   
//                 // case DECSET_ENB_GRAP_ROT_PRINT_MODE:    { printf("TODO: DECSET_ENB_GRAP_ROT_PRINT_MODE\n"); } break;    
//                 case DECSET_APPLICATION_KEYPAD_MODE:    { printf("TODO: DECSET_APPLICATION_KEYPAD_MODE\n"); } break;    
//                 case DECSET_BACKARROW_SENDS_BACKSPACE:  { printf("TODO: DECSET_BACKARROW_SENDS_BACKSPACE\n"); } break;  
//                 case DECSET_LEFT_RIGHT_MARGIN_MODE:     { printf("TODO: DECSET_LEFT_RIGHT_MARGIN_MODE\n"); } break;     
//                 case DECSET_ENB_SIXEL_DISPLAY_MODE:     { printf("TODO: DECSET_ENB_SIXEL_DISPLAY_MODE\n"); } break;     
//                 case DECSET_NOTCLEAR_SCREEN_ON_DECCOLM: { printf("TODO: DECSET_NOTCLEAR_SCREEN_ON_DECCOLM\n"); } break; 
//                 case DECSET_SEND_MOUXY_ON_BUTPRESSRELS: { printf("TODO: DECSET_SEND_MOUXY_ON_BUTPRESSRELS\n"); } break; 
//                 case DECSET_HILITE_MOUSE_TRACKING:      { printf("TODO: DECSET_HILITE_MOUSE_TRACKING\n"); } break;      
//                 case DECSET_CELL_MOTION_MOUSE_TRACKING: { printf("TODO: DECSET_CELL_MOTION_MOUSE_TRACKING\n"); } break; 
//                 case DECSET_ALL_MOTION_MOUSE_TRACKING:  { printf("TODO: DECSET_ALL_MOTION_MOUSE_TRACKING\n"); } break;  
//                 case DECSET_SEND_FOCUS_INOUT_EVENTS:    { printf("TODO: DECSET_SEND_FOCUS_INOUT_EVENTS\n"); } break;    
//                 case DECSET_UTF8_MOUSE_MODE:            { printf("TODO: DECSET_UTF8_MOUSE_MODE\n"); } break;            
//                 case DECSET_SGR_MOUSE_MODE:             { printf("TODO: DECSET_SGR_MOUSE_MODE\n"); } break;             
//                 case DECSET_ALTERNATE_SCROLL_MODE:      { printf("TODO: DECSET_ALTERNATE_SCROLL_MODE\n"); } break;      
//                 case DECSET_SCROLL_BOTTOM_TTY_OUTPUT:   { printf("TODO: DECSET_SCROLL_BOTTOM_TTY_OUTPUT\n"); } break;   
//                 case DECSET_SCROLL_BOTTOM_ON_KEYPRESS:  { printf("TODO: DECSET_SCROLL_BOTTOM_ON_KEYPRESS\n"); } break;  
//                 case DECSET_ENB_FASTSCROLL:             { printf("TODO: DECSET_ENB_FASTSCROLL\n"); } break;             
//                 case DECSET_ENB_URXVT_MOUSE_MODE:       { printf("TODO: DECSET_ENB_URXVT_MOUSE_MODE\n"); } break;       
//                 case DECSET_ENB_SGR_MOUSE_PIXELMODE:    { printf("TODO: DECSET_ENB_SGR_MOUSE_PIXELMODE\n"); } break;    
//                 case DECSET_INTERPRET_META_KEY:         { printf("TODO: DECSET_INTERPRET_META_KEY\n"); } break;         
//                 case DECSET_ENB_SPCMOD_ALT_NUMLOCK:     { printf("TODO: DECSET_ENB_SPCMOD_ALT_NUMLOCK\n"); } break;     
//                 case DECSET_SEND_ESC_WHEN_META_MOD:     { printf("TODO: DECSET_SEND_ESC_WHEN_META_MOD\n"); } break;     
//                 case DECSET_SEND_DEL_FROM_EDITKEYPAD:   { printf("TODO: DECSET_SEND_DEL_FROM_EDITKEYPAD\n"); } break;   
//                 case DECSET_SEND_ESC_WHEN_ALT_MOD:      { printf("TODO: DECSET_SEND_ESC_WHEN_ALT_MOD\n"); } break;      
//                 case DECSET_KEEP_SELEC_NOT_HIGLIG:      { printf("TODO: DECSET_KEEP_SELEC_NOT_HIGLIG\n"); } break;      
//                 case DECSET_USE_CLIPBOARD_SELECTION:    { printf("TODO: DECSET_USE_CLIPBOARD_SELECTION\n"); } break;    
//                 case DECSET_ENB_URGWIN_ON_CTRL_G:       { printf("TODO: DECSET_ENB_URGWIN_ON_CTRL_G\n"); } break;       
//                 case DECSET_ENB_RAISEWIN_ON_CTRL_G:     { printf("TODO: DECSET_ENB_RAISEWIN_ON_CTRL_G\n"); } break;     
//                 case DECSET_REUSE_MOST_RECENT_CLIPBRD:  { printf("TODO: DECSET_REUSE_MOST_RECENT_CLIPBRD\n"); } break;  
//                 case DECSET_EXTENDED_REVERSE_WRAP_MODE: { printf("TODO: DECSET_EXTENDED_REVERSE_WRAP_MODE\n"); } break; 
//                 case DECSET_ENB_SWAP_ALT_SCREEN_BUF:    { printf("TODO: DECSET_ENB_SWAP_ALT_SCREEN_BUF\n"); } break;    
//                 case DECSET_USE_ALT_SCREEN_BUFFER:      { printf("TODO: DECSET_USE_ALT_SCREEN_BUFFER\n"); } break;      
//                 case DECSET_SAVE_CURSOR:                { printf("TODO: DECSET_SAVE_CURSOR\n"); } break;                
//                 case DECSET_SAVE_CURSOR_2:              { printf("TODO: DECSET_SAVE_CURSOR_2\n"); } break;              
//                 case DECSET_TERMINFOCAP_FNKEY_MODE:     { printf("TODO: DECSET_TERMINFOCAP_FNKEY_MODE\n"); } break;     
//                 case DECSET_SET_SUN_FUNCKEY_MODE:       { printf("TODO: DECSET_SET_SUN_FUNCKEY_MODE\n"); } break;       
//                 case DECSET_SET_HP_FUNCKEY_MODE:        { printf("TODO: DECSET_SET_HP_FUNCKEY_MODE\n"); } break;        
//                 case DECSET_SET_SCO_FUNCKEY_MODE:       { printf("TODO: DECSET_SET_SCO_FUNCKEY_MODE\n"); } break;       
//                 case DECSET_SET_LEGACY_KEYBOARD_EMUL:   { printf("TODO: DECSET_SET_LEGACY_KEYBOARD_EMUL\n"); } break;   
//                 case DECSET_SET_VT220_KEYBOARD_EMUL:    { printf("TODO: DECSET_SET_VT220_KEYBOARD_EMUL\n"); } break;    
//                 case DECSET_SET_READLINE_MOUSEBUT_1:    { printf("TODO: DECSET_SET_READLINE_MOUSEBUT_1\n"); } break;    
//                 case DECSET_SET_READLINE_MOUSEBUT_2:    { printf("TODO: DECSET_SET_READLINE_MOUSEBUT_2\n"); } break;    
//                 case DECSET_SET_READLINE_MOUSEBUT_3:    { printf("TODO: DECSET_SET_READLINE_MOUSEBUT_3\n"); } break;    
//                 case DECSET_SET_BRACKETED_PASTE_MODE:   { printf("TODO: DECSET_PASTER_BRACKETED_MODE\n"); m->bracket_mode = b == 'h'; } break;   
//                 case DECSET_ENB_READLINE_CHARQUOTE:     { printf("TODO: DECSET_ENB_READLINE_CHARQUOTE\n"); } break;     
//                 case DECSET_ENB_READLINE_NEWLINE_PASTE: { printf("TODO: DECSET_ENB_READLINE_NEWLINE_PASTE\n"); } break; 
//                 default: printf("UNKNOWN DECSET OP: %d\n", opt);
//             }
//
//             return;
//         }
//
//
//         case SAVE_CURSOR: {
//             sv_chop_left(bytes);
//             m->saved_cursor = m->cursor;
//             return;
//         }
//
//         case RESTORE_CURSOR: {
//             sv_chop_left(bytes);
//             m->cursor = m->saved_cursor;
//             return;
//         }
//     }
//
//     int argv[10] = {-1};
//     int argc     = 0;
//     minal_parse_ansi_args(m, bytes, &argc, argv);
//
//     b = sv_chop_left(bytes);
//     switch (b) {
//
//         case CURSOR_UP: {
//             int opt = argc > 0 ? argv[0] : 1;
//
//             size_t new_row;
//
//             if (m->cursor.row < opt || m->cursor.row - opt <= m->reg_top) {
//                 new_row = m->reg_top;
//             } else {
//                 new_row = m->cursor.row - opt;
//             }
//             minal_cursor_move(m, m->cursor.col, new_row);
//         }; break;
//
//         case CURSOR_DOWN: {
//             int opt = argc > 0 ? argv[0] : 1;
//             size_t new_row;
//             if (m->cursor.row + opt >= m->reg_bot) {
//                 new_row = m->reg_bot - 1;
//             } else {
//                 new_row = m->cursor.row + opt;
//             }
//             minal_cursor_move(m, m->cursor.col, new_row);
//         }; break;
//
//         case CURSOR_FORWARD: {
//             int opt = argc > 0 ? argv[0] : 1;
//
//             size_t new_col;
//             if (m->cursor.col + opt >= m->config.n_cols) {
//                 new_col = m->config.n_cols - 1;
//             } else {
//                 new_col = m->cursor.col + opt;
//             }
//             minal_cursor_move(m, new_col, m->cursor.row);
//         }; break;
//
//         case CURSOR_BACK: {
//             int opt = argc > 0 ? argv[0] : 1;
//
//             size_t new_col;
//             if (m->cursor.col < opt) {
//                 new_col = 0;
//             } else {
//                 new_col = m->cursor.col - opt;
//             }
//             minal_cursor_move(m, new_col, m->cursor.row);
//         }; break;
//
//         case CURSOR_POSITION: {
//             int opt1 = argc > 0 ? argv[0] : 1;
//             int opt2 = argc > 1 ? argv[1] : 1;
//
//             size_t new_col = MIN(MAX(0, opt1 - 1), m->config.n_cols - 1);
//             size_t new_row = MIN(MAX(0, opt2 - 1), m->config.n_rows - 1);
//             minal_cursor_move(m, new_col, new_row);
//         }; break;
//
//         case ERASE_IN_DISPLAY: {
//             size_t opt = argc > 0 ? argv[0] : 0;
//             minal_erase_in_display(m, opt);
//         }; break;
//
//         case ERASE_IN_LINE: {
//             size_t opt = argc > 0 ? argv[0] : 0;
//             minal_erase_in_line(m, opt);
//         }; break;
//
//         case INSERT_LINES: {
// 			printf("TODO: CSI %c\n", INSERT_LINES);
// 		}; break;
//
//         case DELETE_LINES: {
// 			printf("TODO: CSI %c\n", DELETE_LINES);
// 		}; break;
//
//         case DELETE_CHARS: {
// 			printf("TODO: CSI %c\n", DELETE_CHARS);
// 		}; break;
//
//         case SCROLL_UP: {
//             size_t opt = argc > 0 ? argv[0] : 1;
//             minal_pageup(m, opt);
//         }; break;
//
//         case SCROLL_DOWN: {
//             size_t opt = argc > 0 ? argv[0] : 1;
//             minal_pagedown(m, opt);
//         }; break;
//
//         case ERASE_CHARS: {
// 			printf("TODO: CSI %c\n", ERASE_CHARS);
// 		}; break;
//
//         case BACKWARD_TAB: {
// 			printf("TODO: CSI %c\n", BACKWARD_TAB);
// 		}; break;
//
//         case SCROLL_DOWN_2: {
// 			printf("TODO: CSI %c\n", SCROLL_DOWN_2);
// 		}; break;
//
//         case CHAR_POSITION_ABSOLUTE: {
// 			printf("TODO: CSI %c\n", CHAR_POSITION_ABSOLUTE);
// 		}; break;
//
//         case CHAR_POSITION_RELATIVE: {
// 			printf("TODO: CSI %c\n", CHAR_POSITION_RELATIVE);
// 		}; break;
//
//         case REPEAT_PRECED_GRAPHIC_CHAR: {
// 			printf("TODO: CSI %c\n", REPEAT_PRECED_GRAPHIC_CHAR);
// 		}; break;
//
//         case DEVICE_ATTRIBUTES_REPORT: {
// 			printf("TODO: CSI %c\n", DEVICE_ATTRIBUTES_REPORT);
// 		}; break;
//
//         case LINE_POSITION_ABSOLUTE: {
// 			printf("TODO: CSI %c\n", LINE_POSITION_ABSOLUTE);
// 		}; break;
//
//         case LINE_POSITION_RELATIVE: {
// 			printf("TODO: CSI %c\n", LINE_POSITION_RELATIVE);
// 		}; break;
//
//         case HORIZONTAL_VERTICAL_POSITION: {
// 			printf("TODO: CSI %c\n", HORIZONTAL_VERTICAL_POSITION);
// 		}; break;
//
//         case TAB_CLEAR: {
// 			printf("TODO: CSI %c\n", TAB_CLEAR);
// 		}; break;
//
//         case SET_MODE: {
// 			printf("TODO: CSI %c\n", SET_MODE);
// 		}; break;
//         case MEDIA_COPY: {
// 			printf("TODO: CSI %c\n", MEDIA_COPY);
// 		}; break;
//         case RESET_MODE: {
// 			printf("TODO: CSI %c\n", RESET_MODE);
// 		}; break;
//
//         case SELECT_GRAPHIC_RENDITION: {
//             minal_graphic_mode(m, argv, argc);
//         }; break;
//
//         case DEVICE_STATUS_REPORT: {
//             if (argc < 1 && !(argv[0] == DSR_STATUS || argv[0] == DSR_CURSOR_POSITION)) {
//                 printf("INVALID CSI ESCAPE SEQUENCE\n");
//                 return;
//             }
//             if (argv[0] == DSR_STATUS) {
//                 minal_write_str(m, "\x1B[0n");
//             }
//
//             // report cursor position as ESC[row;colR
//             char str[50];
//             sprintf(str, "\x1B[%zu;%zuR", m->cursor.row + 1, m->cursor.col + 1);
//             minal_write_str(m, str);
//         }; break;
//
//         case DEC_SCROLL_TOPBOT_MARGIN: { 
//             size_t top = argc > 0 ? argv[0] - 1 : 0;
//             size_t bot = argc > 1 ? argv[1] - 1 : m->config.n_rows - 1;
//
//             top = MAX(0, top);
//             bot = MIN(m->config.n_rows - 1, bot);
//
//             if (top > bot) {
//                 top = 0;
//                 bot = m->config.n_rows - 1;
//             }
//
//             m->reg_top = top;
//             m->reg_bot = bot;
//             minal_cursor_move(m, 0, 0);
//             minal_erase_in_display(m, 2);
//         }; break;
//
//         case DEC_SCROLL_LEFRIG_MARGIN: { 
//             printf("TODO: CSI: DEC_SCROLL_LEFRIG_MARGIN\n"); 
//         }; break;
//
//         default: {
//             // __asm__("int3");
//             printf("UNKNOW CSI ESCAPE SEQUENCE\n");
//             printf("    OP: %c\n", b);
//             if (argc > 0) {
//                 printf("    ARGS: ");
//                 for (int i = 0; i < argc; ++i) {
//                     printf("%d, ", argv[i]);
//                 }
//                 printf("\n");
//             }
//         }; break;
//     }
//     // printf(" CSI: BEFORE END: 0b%08b - 0x%02X (%c)\n", b, b, b);
//     return;
// }
//
// void minal_parse_ansi(Minal* m, StringView* bytes)
// {
//     uint8_t b = sv_chop_left(bytes);
//     switch (b) {
//
//         case DEC_SAVE_CURSOR: {
//             sv_chop_left(bytes);
//             m->saved_cursor = m->cursor;
//             return;
//         }
//
//         case DEC_RESTORE_CURSOR: {
//             sv_chop_left(bytes);
//             m->cursor = m->saved_cursor;
//             return;
//         }
//
//         case INDEX: {
//             printf("TODO: C1 CODE: INDEX\n");
//             return;
//         }
//
//         case REVERSE_INDEX: {
//             printf("TODO: C1 CODE: REVERSE_INDEX\n");
//             return;
//         }
//
//         case FULL_RESET: {
//             printf("TODO: C1 CODE: FULL_RESET\n");
//             return;
//         }
//
//         case DEC_KEYPAD_APPLICATION_MODE: {
//             printf("TODO: KEYPAD_APPLICATION_MODE\n");
//             m->keypad_mode = KEYPAD_APPLICATION_MODE;
//             return;
//         }
//
//         case DEC_KEYPAD_NORMAL_MODE: {
//             printf("TODO: KEYPAD_NORMAL_MODE\n");
//             m->keypad_mode = KEYPAD_NORMAL_MODE;
//             return;
//         }
//
//         case DEVICE_CONTROL_STRING: {
//             printf("TODO: DEVICE_CONTROL_STRING\n");
//             while(sv_chop_left(bytes) != ESC && sv_first(bytes) != STRING_TERMINATOR);
//             sv_chop_left(bytes);
//             return;
//         }
//
//         case CONTROL_SEQUENCE_INTRODUCER: {
//             minal_parse_ansi_csi(m, bytes);
//             return;
//         }
//
//         case STRING_TERMINATOR: {
//             printf("ERROR: ISOLATED STRING_TERMINATOR\n");
//         }; break;
//
//         case OPERATING_SYSTEM_COMMAND: {
//             minal_parse_ansi_osc(m, bytes);
// 		}; break;
//
//         case PRIVACY_MESSAGE: {
//             printf("TODO: PRIVACY_MESSAGE\n");
// 		}; break;
//
//         case APPLICATION_PROGRAM_COMMAND: {
//             printf("TODO: APPLICATION_PROGRAM_COMMAND\n");
// 		}; break;
//
//         default: printf("UNKNOWN C1 CODE ESCAPE SEQUENCE: 0b%08b | 0x%02X (%c)\n", b, b, b);
//      }
// }
