#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BELL        '\a'
#define DEL         0x7f
#define BACKSPACE   '\b' 
#define FORMFEED    '\f'
#define TAB         '\t'
#define LINEFEED    '\n'
#define CARRIAGERET '\r'
#define ESC         '\033'

typedef enum {
  C1_Control_Invalid = -1,
  C1_Control_IND,              // Index - move cursor down one line
  C1_Control_NEL,              // Next Line - move cursor to beginning of next line
  C1_Control_HTS,              // Horizontal Tab Set - set tab stop at current column
  C1_Control_RI,               // Reverse Index - move cursor up one line
  C1_Control_SS2,              // Single Shift 2 - invoke G2 character set for next character only
  C1_Control_SS3,              // Single Shift 3 - invoke G3 character set for next character only
  C1_Control_DCS,              // Device Control String - device control sequence (followed by ST)
  C1_Control_SPA,              // Start of Guarded Area - begin guarded area
  C1_Control_EPA,              // End of Guarded Area - end guarded area
  C1_Control_SOS,              // Start of String - generic string sequence (followed by ST)
  C1_Control_RTI,              // Return to Index - move cursor up, reverse of IND
  C1_Control_CSI,              // Control Sequence Introducer - introduces a control sequence (ESC[)
  C1_Control_ST,               // String Terminator - terminates a string sequence
  C1_Control_OSC,              // Operating System Command - window title, color, etc (ESC])
  C1_Control_APC,              // Application Program Command - user-defined (followed by ST)

  // VT100 special sequences (2-byte after ESC)
  C1_Control_VT100_SP,        // 2-byte control after ESC (space character)
  C1_Control_VT100_DEC,       // DEC-specific control (# followed by char)
  C1_Control_VT100_SEL,       // Selection control (% followed by char)
  C1_Control_VT100_G0,        // Designate G0 character set (ESC( )
  C1_Control_VT100_G1,        // Designate G1 character set (ESC) )
  C1_Control_VT100_G2,        // Designate G2 character set (ESC* )
  C1_Control_VT100_G3,        // Designate G3 character set (ESC+ )

  // Special VT100 sequences
  C1_Control_Special_Start,
  C1_Control_Special_DECSC = C1_Control_Special_Start, // Save Cursor Position (ESC 7)
  C1_Control_Special_DECRC,                            // Restore Cursor Position (ESC 8)
  C1_Control_Special_DECPAM,                           // Application Keypad Mode (ESC =)
  C1_Control_Special_DECPNM,                           // Normal Keypad Mode (ESC >)
  C1_Control_Special_CTLLCS,                           // Cursor to Lower-Left Corner (ESC F)
  C1_Control_Special_RIS,                              // Full Reset (ESC c)
  C1_Control_Special_ML,                               // Memory Lock (ESC l)
  C1_Control_Special_MU,                               // Memory Unlock (ESC m)
  C1_Control_Special_LS2,                              // Lock Shift 2 (ESC n)
  C1_Control_Special_LS3,                              // Lock Shift 3 (ESC o)
  C1_Control_Special_LS3R,                             // Lock Shift 3 Right (ESC |)
  C1_Control_Special_LS2R,                             // Lock Shift 2 Right (ESC })
  C1_Control_Special_LS1R,                             // Lock Shift 1 Right (ESC ~)
  C1_Control_Special_Max = C1_Control_Special_LS1R,

  C1_Control__MaxValid = C1_Control_Special_Max,
  C1_Control__Length,
} C1_Control;

typedef enum {
  VT100_CharSet_Invalid = -1,
  VT100_CharSet_Special_LineDrawing, //0      -> DEC Special Character and Line Drawing Set
  VT100_CharSet_UnitedKingdom,       //A      -> United Kingdom (UK)
  VT100_CharSet_UnitedStates,        //B      -> United States (USASCII)
  VT100_CharSet_Dutch,               //4      -> Dutch
  VT100_CharSet_Finnish,             //C or 5 -> Finnish
  VT100_CharSet_French,              //R      -> French
  VT100_CharSet_FrenchCanadian,      //Q      -> French Canadian
  VT100_CharSet_German,              //K      -> German
  VT100_CharSet_Italian,             //Y      -> Italian
  VT100_CharSet_Norwegian,           //E or 6 -> Norwegian/Danish
  VT100_CharSet_Spanish,             //Z      -> Spanish
  VT100_CharSet_Swidsh,              //H or 7 -> Swedish
  VT100_CharSet_Swiss,               //=      -> Swiss
  VT100_CharSet__MaxValid = VT100_CharSet_Swiss,
  VT100_CharSet__Length
} VT100_CharSet;

const char* ansi_c1_control_to_str(C1_Control ctrl);

void ansi_debug(const char* data, size_t len);
bool ansi_find_cmd_end(const char* data, size_t len, size_t* out_len, C1_Control* out_ctrl);
int ansi_str_to_int(const char* str, size_t len, int def);

enum {
  ANSI_State_Unset,  // state did not change
  ANSI_State_Set,    // state should be set
  ANSI_State_Reset,  // state should be reset
  ANSI_State_Alt_Set // state should be set to alternative state (ex: underline -> double underline)
};

// Erase modes for clear_screen/clear_line
// CSI Ps J (ED) / CSI Ps K (EL) - ctlseqs.txt lines 230-248
typedef enum {
  ANSI_Erase_Invalid = -1,
  ANSI_Erase_ToEnd = 0,       // Erase Below / Erase to Right (default)
  ANSI_Erase_ToBegin = 1,     // Erase Above / Erase to Left
  ANSI_Erase_All = 2,        // Erase All
  ANSI_Erase_Saved = 3        // Erase Saved Lines (xterm only) - for ED only
} ANSI_EraseMode;

typedef enum {
  ControlCmd_Invalid = -1,
  ControlCmd_Action, // Clear screen/line, scroll, save/restore cursor
  ControlCmd_Style,  // CSI <...> m - SGR style
  ControlCmd_Window, // Window/scroll region
  ControlCmd_Cursor, // Cursor movement
  ControlCmd_OSC,    // OSC - Operating System Commands
  ControlCmd_DSR,    // Device Status Report
  ControlCmd_Fill,   // Rectangle fill/erase
  ControlCmd_Feature // DECSET/DECRST modes
} ControlCmd;

// Standard 16-color palette (ANSI colors)
static const uint8_t ANSI_ColorPalette[16][3] = {
  [0] = { 0x00, 0x00, 0x00 }, // black
  [1] = { 0xCD, 0x00, 0x00 }, // red
  [2] = { 0x00, 0xCD, 0x00 }, // green
  [3] = { 0xCD, 0xCD, 0x00 }, // yellow
  [4] = { 0x00, 0x00, 0xCD }, // blue
  [5] = { 0xCD, 0x00, 0xCD }, // magenta
  [6] = { 0x00, 0xCD, 0xCD }, // cyan
  [7] = { 0xCD, 0xCD, 0xCD }, // white/bright black (30-37)
  [8] = { 0x7F, 0x7F, 0x7F }, // bright black
  [9] = { 0xFF, 0x00, 0x00 }, // bright red
  [10] = { 0x00, 0xFF, 0x00 }, // bright green
  [11] = { 0xFF, 0xFF, 0x00 }, // bright yellow
  [12] = { 0x00, 0x00, 0xFF }, // bright blue
  [13] = { 0xFF, 0x00, 0xFF }, // bright magenta
  [14] = { 0x00, 0xFF, 0xFF }, // bright cyan
  [15] = { 0xFF, 0xFF, 0xFF }, // bright white
};

// Default foreground/background colors
static const uint8_t ANSI_DefaultFg[3] = { 0xCD, 0xCD, 0xCD };
static const uint8_t ANSI_DefaultBg[3] = { 0x00, 0x00, 0x00 };

typedef struct {
  uint8_t r,g,b; 
} ANSI_Color;

typedef struct {
  uint8_t reset :1; // if set: should reset all the states to default before applying any other change
  
  /* Font tyles */
  uint8_t bold  :2; 
  uint8_t italic:2; 

  /* text decorations */
  uint8_t underline    :2;
  uint8_t overline     :2;
  uint8_t strikethrough:2; 

  uint8_t faint    :2;
  uint8_t blink    :2;
  uint8_t inverse  :2;
  uint8_t invisible:2;

  uint8_t fg_change:2;
  uint8_t bg_change:2;
  uint8_t ul_change:2; 
   
  ANSI_Color fg_color;
  ANSI_Color bg_color;
  ANSI_Color ul_color;
} ANSI_Style_Cmd;


typedef struct {


  // window change CSI 8 ; <h> ; <w> t
  uint8_t window_resize: 1; //if set, width and height define the changed size

  // scroll region command CSI <top> ; <bottom> r
  uint8_t scroll_region:2; //if set: the scroll regions was changed; if reset: restore default scroll region
  
  uint8_t warning_bell: 1;  // if set: bell_volume.warning is the volume
  uint8_t  margin_bell: 1;  // if set: bell_volume.margni is the volume

  struct { uint16_t top, bottom; }   scroll; // set scroll region
  struct { uint8_t left, right;  }   margin; // set margin
  struct { uint16_t width, height; } window; // set window size (in chars)
  struct { uint8_t warning, margin; } bell_volume;

} ANSI_Window_Cmd;

typedef struct {
  char *title;        // OSC 0 / 2
  char *clipboard;    // OSC 52
} ANSI_OSC_Cmd;

typedef struct {
  uint8_t status              :1; // CSI 5 n -> if set report status. Result OK is CSI 0 n
  uint8_t cursor              :1; // CSI 6 n -> if set report cursor position CSI <row> ; <col> R
  uint8_t full_mouse_report   :1; // CSI <n?> ' | -> if set: sends as response CSI <event>;<button>;<row>;<col>;<page> & w; full description at ./ctlseqs.txt:876
  uint8_t enable_mouse_report :2; // CSI <State>;<Mode> ' z -> if set: enables mouse report; if alt set: reports only once
  uint8_t mouse_report_pixels :1; // if set: reports position in pixels otherwise, reports position in character cells
  uint8_t select_events       :1; // if set: mouse_selected_event shows the current selected event: see .                                        /ctlseqs:862
  uint8_t mouse_selected_event:3; // events from 0 to 4
} ANSI_DeviceStatusReport_Cmd;


typedef struct {
  uint8_t fill_rectangle  :1; // CSI <top>;<left>;<bottom>;<right> $ x -> if set: fill the rectangle with fill_character
  uint8_t copy_rectangle  :1; // CSI <top>;<left>;<bottom>;<right>;<src page>;<dest top>;<dest left>;<dest page> $ v -> if set: copy the rectangle to dest_position
  uint8_t erase_rectangle :1; // CSI <top>;<left>;<bottom>;<right> [ $ z or $ { ]  -> if set: erase the rectangle

  uint8_t revert_attr_rectangle :1; // CSI <top>;<left>;<bottom>;<right>;<attr> $ t -> if set: reverse attribute revert_attr (only) inside rectangle
  uint8_t revert_attr           :3; // acceptable 1,4,5,7 and 8
  uint8_t change_attr_rectangle :1; // CSI <top>;<left>;<bottom>;<right>;<attr...> $ r -> if set: change attribute to change_attr inside rectangle

  struct { uint16_t top,left,bottom,right;  } rectangle;
  struct { uint16_t top,left;  } dest_position;
  uint32_t fill_character;
  ANSI_Style_Cmd change_attr;

} ANSI_Fill_Cmd;



typedef enum {
  Cursor_Block,     //CSI [0,1,2] SP q
  Cursor_Underline, //CSI [3,4  ] SP q
  Cursor_Bar,       //CSI [5,6  ] SP q
} CursorShape;

typedef struct {
  uint8_t move_relative:1; // if set moves cursor with relative_motion
  uint8_t move_absolute:1; // if set mover cursor with absolute_motion
  uint8_t visible      :2; // sets or resets the cursor visible state ;
  uint8_t blink        :2; // sets or resets the blinking state; NOTE: shape changes can also affect blinking state

  //CSI s: save; CSI u: restore 
  uint8_t save_cursor:2; // if set: should save the current cursor position; if reset: sould respore the saved cursor position

  struct { uint16_t row, col; } relative_motion, absolute_motion; // 1 based: 0 means current row or col
  CursorShape shape;
  uint16_t forward_tabulation;
  /* DECSC save/restore */
} ANSI_Cursor_Cmd;


// DEC Private Mode Set (DECSET).
typedef struct {
  uint8_t cursor_keys:2;                        // Ps = 1     -> Application Cursor Keys (DECCKM).
  uint8_t us_ascii_vt100:2;                     // Ps = 2     -> Designate USASCII for character sets G0-G3 (DECANM), and set VT100 mode.
  uint8_t enable_cols132:2;                     // Ps = 3     -> 132 Column Mode (DECCOLM).
  uint8_t smooth_scroll:2;                      // Ps = 4     -> Smooth (Slow) Scroll (DECSCLM).
  uint8_t reverse_video:2;                      // Ps = 5     -> Reverse Video (DECSCNM).
  uint8_t cursor_relative:2;                    // Ps = 6     -> Origin Mode (DECOM). when enabled cursor (1,1) is relative to scroll region,
                                                //                                    when disabled (1,1) is relative to window
  uint8_t wraparround_mode:2;                   // Ps = 7     -> Wraparound Mode (DECAWM).
  uint8_t autorepeat_keys:2;                    // Ps = 8     -> Auto-repeat Keys (DECARM).
  uint8_t mouse_tracking_on_press:2;            // Ps = 9     -> Send Mouse X & Y on button press.  See the section Mouse Tracking.
  uint8_t show_toolbar:2;                       // Ps = 10    -> Show toolbar (rxvt).
  uint8_t start_blinking_cursor:2;              // Ps = 12    -> Start Blinking Cursor (att610).
  uint8_t print_form_feed:2;                    // Ps = 18    -> Print form feed (DECPFF).
  uint8_t print_extend_fullscreen:2;            // Ps = 19    -> Set print extent to full screen (DECPEX).
  uint8_t show_cursor:2;                        // Ps = 25    -> Show Cursor (DECTCEM).
  uint8_t show_scrollbar:2;                     // Ps = 30    -> Show scrollbar (rxvt).
  uint8_t enable_font_shifting:2;               // Ps = 35    -> Enable font-shifting functions (rxvt).
  uint8_t _enter_tektronix_mode:2;              // Ps = 38    -> Enter Tektronix Mode (DECTEK).
  uint8_t _allow_80:2;                          // Ps = 40    -> Allow 80 -> 132 Mode.
  uint8_t _more:2;                              // Ps = 41    -> more(1) fix (see curses resource).
  uint8_t _nation_replaceemnt_charset:2;        // Ps = 42    -> Enable Nation Replacement Character sets (DECN-RCM).
  uint8_t turn_margin_bell:2;                   // Ps = 44    -> Turn On Margin Bell.
  uint8_t reverse_wraparrownd_mode:2;           // Ps = 45    -> Reverse-wraparound Mode.
  uint8_t _start_logging:2;                     // Ps = 46    -> Start Logging.  This is normally disabled by acompile-time option.
  uint8_t enable_alt_screen0:2;                 // Ps = 47    -> Use Alternate Screen Buffer.  (This may be disabled by the titeInhibit resource).
  uint8_t enable_keypad:2;                      // Ps = 66    -> Application keypad (DECNKM).
  uint8_t enable_backarrow_as_backspace:2;      // Ps = 67    -> Backarrow key sends backspace (DECBKM).
  uint8_t enable_margin_mode:2;                 // Ps = 69    -> Enable left and right margin mode (DECLRMM), VT420 and up.
  uint8_t disable_clear_on_cols132_toggle:2;    // Ps = 95    -> Do not clear screen when DECCOLM is set/reset (DECNCSM), VT510 and up.
  uint8_t mouse_tracking_on_pess_and_release:2; // Ps = 1000  -> Send Mouse X & Y on button press and release.  See the section Mouse Tracking.
  uint8_t mouse_tracking_hilite:2;              // Ps = 1001  -> Use Hilite Mouse Tracking.
  uint8_t mouse_tracking_cell_motion:2;         // Ps = 1002  -> Use Cell Motion Mouse Tracking.
  uint8_t mouse_tracking_all_motion:2;          // Ps = 1003  -> Use All Motion Mouse Tracking.
  uint8_t enable_focus_events:2;                // Ps = 1004  -> Send FocusIn/FocusOut events.
  uint8_t enable_utf8_mouse_mode:2;             // Ps = 1005  -> Enable UTF-8 Mouse Mode.
  uint8_t enable_SRG_mouse_mode:2;              // Ps = 1006  -> Enable SGR Mouse Mode.
  uint8_t enable_alt_scroll_mode:2;             // Ps = 1007  -> Enable Alternate Scroll Mode.
  uint8_t enable_autoscroll_on_output:2;        // Ps = 1010  -> Scroll to bottom on tty output (rxvt).
  uint8_t enable_urxvt_mouse_mode:2;            // Ps = 1015  -> Enable urxvt Mouse Mode.
  uint8_t enable_autoscroll_on_keypress:2;      // Ps = 1011  -> Scroll to bottom on key press (rxvt).
  uint8_t enable_eightBitInput:2;               // Ps = 1034  -> Interpret "meta" key, sets eighth bit. (enables the eightBitInput resource).
  uint8_t enable_numLock:2;                     // Ps = 1035  -> Enable special modifiers for Alt and NumLock keys.  (This enables the numLock resource).
  uint8_t enable_metaSendEscape:2;              // Ps = 1036  -> Send ESC   when Meta modifies a key.  (This enables the metaSendsEscape resource).
  uint8_t send_del_from_delete_key:2;           // Ps = 1037  -> Send DEL from the editing-keypad Delete key.
  uint8_t enable_altSendsEscape:2;              // Ps = 1039  -> Send ESC  when Alt modifies a key.  (This enables the altSendsEscape resource).
  uint8_t enable_keepSelection:2;               // Ps = 1040  -> Keep selection even if not highlighted. (This enables the keepSelection resource).
  uint8_t enabel_selectToClipboard:2;           // Ps = 1041  -> Use the CLIPBOARD selection.  (This enables the selectToClipboard resource).
  uint8_t enable_bellIsUrgent:2;                // Ps = 1042  -> Enable Urgency window manager hint when Control-G is received.  (This enables the bellIsUrgent resource).
  uint8_t enable_popOnBell:2;                   // Ps = 1043  -> Enable raising of the window when Control-G is received.  (enables the popOnBell resource).
  uint8_t enable_alt_screen1:2;                 // Ps = 1047  -> Use Alternate Screen Buffer.  (This may be disabled by the titeInhibit resource).
  uint8_t save_cursor:2;                        // Ps = 1048  -> Save cursor as in DECSC.  (This may be disabled by the titeInhibit resource).
  uint8_t enable_alt_scree_and_save_cursor:2;   // Ps = 1049  -> Save cursor as in DECSC and use Alternate Screen Buffer, clearing it first. (This may be disabled by the titeInhibit resource). This combines the effects of the 1047  and 1048  modes.  Use this with terminfo-based applications rather than the 47 mode.
  uint8_t _term_fnkey_mode:2;                   // Ps = 1050  -> Set terminfo/termcap function-key mode.
  uint8_t _sun_fnkey_mode:2;                    // Ps = 1051  -> Set Sun function-key mode.
  uint8_t _hp_fnkey_mode:2;                     // Ps = 1052  -> Set HP function-key mode.
  uint8_t _sco_fnkey_mode:2;                    // Ps = 1053  -> Set SCO function-key mode.
  uint8_t _legacy_keyboard:2;                   // Ps = 1060  -> Set legacy keyboard emulation (X11R6).
  uint8_t _vt220_keyboard:2;                    // Ps = 1061  -> Set VT220 keyboard emulation.
  uint8_t enable_bracketed_paste_mode:2;        // Ps = 2004  -> Set bracketed paste mode.
} ANSI_Feature_Cmd;

// Basic action commands (clear, scroll, save/restore)
typedef struct {
  // states
  uint8_t clear_screen       :1; // ESC[<n>J
  uint8_t clear_line         :1; // ESC[<n>K
  uint8_t soft_reset         :1; // ESC[!p
  uint8_t save_restore_cursor:2; // CSI s=Set | u=Reset, uses ANSI_State

  // complement
  ANSI_EraseMode clear_screen_mode; // N in ESC[<n>J default 0
  ANSI_EraseMode clear_line_mode;   // N in ESC[<n>K default 0
  int16_t scroll;                   // ESC[<n>S positive=up, negative=down
  int16_t save_restore_index;       // of present will say what index needs to be saved/restored
} ANSI_Action_Cmd;

// Invalid/unimplemented command - stores pointer to raw sequence
typedef struct {
  const char* data;
  size_t len;
} ANSI_Location;

typedef struct {
  ANSI_Location* items;
  size_t len, cap;
} ANSI_Invalid_Cmd;

// Int list for parsing N0;N1;N2 style strings
typedef struct {
  int* items;
  size_t len, cap;
} ANSI_IntList;

// Main tagged union for decoded commands
typedef struct {
  ANSI_Invalid_Cmd invalid;
  ANSI_Action_Cmd action;
  ANSI_Style_Cmd style;
  ANSI_Cursor_Cmd cursor;
  ANSI_Window_Cmd window;
  ANSI_Feature_Cmd feature;
  ANSI_DeviceStatusReport_Cmd dsr;
  ANSI_Fill_Cmd fill;
  ANSI_IntList _args;
} ANSI_Cmd;

// Decode escape sequence into typed command
bool ansi_decode_cmd(ANSI_Cmd* cmd, const char* data, size_t len);
void ansi_debug_cmd(const ANSI_Cmd* cmd);

bool ansi_parse_int_list(const char* data, size_t len, ANSI_IntList* out, int def);

bool ansi_split_args(const char* data, size_t len, const char** out_arg, size_t* out_len);
