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

void ansi_debug(const char* data, size_t len);
bool ansi_find_cmd_end(const char* data, size_t len, size_t* out_len);
int ansi_str_to_int(const char* str, size_t len, int def);
bool ansi_split_args(const char* data, size_t len, const char** out, size_t* out_len);
bool ansi_split_int_args(const char* data, size_t len, int* out_n, int def);




enum {
  ANSI_State_Unset,  // state did not change
  ANSI_State_Set,    // state should be set
  ANSI_State_Reset,  // state should be reset
  ANSI_State_Alt_Set // state should be set to alternative state (ex: underline -> double underline)
};

typedef enum {
  ControlCmd_Invalid = -1,
  ControlCmd_Style,  // A sequence that changes the style properties CSI <...> m
  ControlCmd_Window, // A sequence that changes the window proprties
  ControlCmd_Cursor, // A sequence that changes the cursor properties
  ControlCmd_OSC,    // A sequence that receives Operating System Commands
  ControlCmd_DSR,    // A sequence that changes Device Status Report options
  ControlCmd_Fill,   // A sequence that performs a rectangle fill operation
  ControlCmd_Feature // A sequence that enables or disables a terminal feature
} ControlCmd;


typedef struct {
  uint8_t reset :1; // if set: should reset all the states to default before applying any other change
  
  /* Font tyles */
  uint8_t bold  :2; // toogles bold state 
  uint8_t italic:2; // toogles italic state 

  /* text decorations */
  uint8_t underline       :2; // if set: underline; if alt set: double underline
  uint8_t overline        :2; // toogle overline state (may or maynot be suported)
  uint8_t strikethrough   :2; // if set 

  uint8_t faint:2;     // sets or resets the faint state: should reduce the brightness of fg_color by 50%
  uint8_t blink:2;     // if set: blink; if alt set: rapid blinking
  uint8_t inverse:2;   // sets of resets the inverse state: should invert fg and bg colors
  uint8_t invisible:2; // sets of resets the invisible state

  uint8_t fg_change:2; // sets or resets the foreground color: fg_color is the new color, if reset use your defaults
  uint8_t bg_change:2; // sets or resets the background color: bg_color is the new color, if reset use your defaults 
  uint8_t ul_change:2; // sets or resets the underline  color: ul_color is the new color, if reset use your defaults
  
  struct { uint8_t r,g,b; } fg_color;
  struct { uint8_t r,g,b; } bg_color;
  struct { uint8_t r,g,b; } ul_color;
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
  uint8_t status:1;  // CSI 5 n -> if set report status. Result OK is CSI 0 n
  uint8_t cursor:1;  // CSI 6 n -> if set report cursor position CSI <row> ; <col> R
  uint8_t full_mouse_report: 1; // CSI <n?> ' | -> if set: sends as response CSI <event>;<button>;<row>;<col>;<page> & w; full description at ./ctlseqs.txt:876

  uint8_t enable_mouse_report: 2; // CSI <State>;<Mode> ' z -> if set: enables mouse report; if alt set: reports only once
  uint8_t mouse_report_pixels: 1; // if set: reports position in pixels otherwise, reports position in character cells
  uint8_t select_events: 1;       // if set: mouse_selected_event shows the current selected event: see ./ctlseqs:862
  uint8_t mouse_selected_event: 3; // events from 0 to 4
} ANSI_DeviceStatusReport_Cmd;


typedef struct {
  uint8_t fill_rectangle  :1; // CSI <top>;<left>;<bottom>;<right> $ x -> if set: fill the rectangle with fill_character
  uint8_t copy_rectangle  :1; // CSI <top>;<left>;<bottom>;<right>;<src page>;<dest top>;<dest left>;<dest page> $ v -> if set: copy the rectangle to dest_position
  uint8_t erase_rectangle :1; // CSI <top>;<left>;<bottom>;<right> [ $ z or $ { ]  -> if set: erase the rectangle
  
  uint8_t revert_attr_rectangle: 1; // CSI <top>;<left>;<bottom>;<right>;<attr> $ t -> if set: reverse attribute revert_attr (only) inside rectangle
  uint8_t revert_attr:3;            // acceptable 1,4,5,7 and 8

  uint8_t change_attr_rectangle: 1; // CSI <top>;<left>;<bottom>;<right>;<attr...> $ r -> if set: change attribute to change_attr inside rectangle

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
  uint8_t move_relative   :1; // if set: moves cursor with relative_motion
  uint8_t move_absolute   :1; // if set: mover cursor with absolute_motion
  uint8_t visible:2; //if set 
  uint8_t blink  :2; // sets or resets the blinking state; NOTE: shape changes can also affect blinking state


  //CSI s: save; CSI u: restore 
  uint8_t save_cursor:2; // if set: should save the current cursor position; if reset: sould respore the saved cursor position

  struct { uint16_t row, col; } relative_motion, absolute_motion; // 1 based: 0 means current row or col
  CursorShape shape;
  uint16_t forward_tabulation;
  /* DECSC save/restore */
} ANSI_Cursor_Cmd;


// DEC Private Mode Set (DECSET).
typedef struct {
  uint8_t cursor_keys:2;                        // Ps = 1  -> Application Cursor Keys (DECCKM).
  uint8_t us_ascii_vt100:2;                     // Ps = 2  -> Designate USASCII for character sets G0-G3 (DECANM), and set VT100 mode.
  uint8_t enable_cols132:2;                     // Ps = 3  -> 132 Column Mode (DECCOLM).
  uint8_t smooth_scroll:2;                      // Ps = 4  -> Smooth (Slow) Scroll (DECSCLM).
  uint8_t reverse_video:2;                      // Ps = 5  -> Reverse Video (DECSCNM).
  uint8_t cursor_relative:2;                    // Ps = 6  -> Origin Mode (DECOM). when enabled cursor (1,1) is relative to scroll region,
                                                //                                 when disabled (1,1) is relative to window
  uint8_t wraparround_mode:2;                   // Ps = 7  -> Wraparound Mode (DECAWM).
  uint8_t autorepeat_keys:2;                    // Ps = 8  -> Auto-repeat Keys (DECARM).
  uint8_t mouse_tracking_on_press:2;            // Ps = 9  -> Send Mouse X & Y on button press.  See the section Mouse Tracking.
  uint8_t show_toolbar:2;                       // Ps = 1 0  -> Show toolbar (rxvt).
  uint8_t start_blinking_cursor:2;              // Ps = 1 2  -> Start Blinking Cursor (att610).
  uint8_t print_form_feed:2;                    // Ps = 1 8  -> Print form feed (DECPFF).
  uint8_t print_extend_fullscreen:2;            // Ps = 1 9  -> Set print extent to full screen (DECPEX).
  uint8_t show_cursor:2;                        // Ps = 2 5  -> Show Cursor (DECTCEM).
  uint8_t show_scrollbar:2;                     // Ps = 3 0  -> Show scrollbar (rxvt).
  uint8_t enable_font_shifting:2;               // Ps = 3 5  -> Enable font-shifting functions (rxvt).
  uint8_t _enter_tektronix_mode:2;              // Ps = 3 8  -> Enter Tektronix Mode (DECTEK).
  uint8_t _allow_80:2;                          // Ps = 4 0  -> Allow 80 -> 132 Mode.
  uint8_t _more:2;                              // Ps = 4 1  -> more(1) fix (see curses resource).
  uint8_t _nation_replaceemnt_charset:2;        // Ps = 4 2  -> Enable Nation Replacement Character sets (DECN-RCM).
  uint8_t turn_margin_bell:2;                   // Ps = 4 4  -> Turn On Margin Bell.
  uint8_t reverse_wraparrownd_mode:2;           // Ps = 4 5  -> Reverse-wraparound Mode.
  uint8_t _start_logging:2;                     // Ps = 4 6  -> Start Logging.  This is normally disabled by acompile-time option.
  uint8_t enable_alt_screen0:2;                 // Ps = 4 7  -> Use Alternate Screen Buffer.  (This may be disabled by the titeInhibit resource).
  uint8_t enable_keypad:2;                      // Ps = 6 6  -> Application keypad (DECNKM).
  uint8_t enable_backarrow_as_backspace:2;      // Ps = 6 7  -> Backarrow key sends backspace (DECBKM).
  uint8_t enable_margin_mode:2;                 // Ps = 6 9  -> Enable left and right margin mode (DECLRMM), VT420 and up.
  uint8_t disable_clear_on_cols132_toggle:2;    // Ps = 9 5  -> Do not clear screen when DECCOLM is set/reset (DECNCSM), VT510 and up.
  uint8_t mouse_tracking_on_pess_and_release:2; // Ps = 1 0 0 0  -> Send Mouse X & Y on button press and release.  See the section Mouse Tracking.
  uint8_t mouse_tracking_hilite:2;              // Ps = 1 0 0 1  -> Use Hilite Mouse Tracking.
  uint8_t mouse_tracking_cell_motion:2;         // Ps = 1 0 0 2  -> Use Cell Motion Mouse Tracking.
  uint8_t mouse_tracking_all_motion:2;          // Ps = 1 0 0 3  -> Use All Motion Mouse Tracking.
  uint8_t enable_focus_events:2;                // Ps = 1 0 0 4  -> Send FocusIn                       /FocusOut events.
  uint8_t enable_utf8_mouse_mode:2;             // Ps = 1 0 0 5  -> Enable UTF-8 Mouse Mode.
  uint8_t enable_SRG_mouse_mode:2;              // Ps = 1 0 0 6  -> Enable SGR Mouse Mode.
  uint8_t enable_alt_scroll_mode:2;             // Ps = 1 0 0 7  -> Enable Alternate Scroll Mode.
  uint8_t enable_autoscroll_on_output:2;        // Ps = 1 0 1 0  -> Scroll to bottom on tty output (rxvt).
  uint8_t enable_urxvt_mouse_mode:2;            // Ps = 1 0 1 5  -> Enable urxvt Mouse Mode.
  uint8_t enable_autoscroll_on_keypress:2;      // Ps = 1 0 1 1  -> Scroll to bottom on key press (rxvt).
  uint8_t enable_eightBitInput:2;               // Ps = 1 0 3 4  -> Interpret "meta" key, sets eighth bit. (enables the eightBitInput resource).
  uint8_t enable_numLock:2;                     // Ps = 1 0 3 5  -> Enable special modifiers for Alt and NumLock keys.  (This enables the numLock resource).
  uint8_t enable_metaSendEscape:2;              // Ps = 1 0 3 6  -> Send ESC   when Meta modifies a key.  (This enables the metaSendsEscape resource).
  uint8_t send_del_from_delete_key:2;           // Ps = 1 0 3 7  -> Send DEL from the editing-keypad Delete key.
  uint8_t enable_altSendsEscape:2;              // Ps = 1 0 3 9  -> Send ESC  when Alt modifies a key.  (This enables the altSendsEscape resource).
  uint8_t enable_keepSelection:2;               // Ps = 1 0 4 0  -> Keep selection even if not highlighted. (This enables the keepSelection resource).
  uint8_t enabel_selectToClipboard:2;           // Ps = 1 0 4 1  -> Use the CLIPBOARD selection.  (This enables the selectToClipboard resource).
  uint8_t enable_bellIsUrgent:2;                // Ps = 1 0 4 2  -> Enable Urgency window manager hint when Control-G is received.  (This enables the bellIsUrgent resource).
  uint8_t enable_popOnBell:2;                   // Ps = 1 0 4 3  -> Enable raising of the window when Control-G is received.  (enables the popOnBell resource).
  uint8_t enable_alt_screen1:2;                 // Ps = 1 0 4 7  -> Use Alternate Screen Buffer.  (This may be disabled by the titeInhibit resource).
  uint8_t save_cursor:2;                        // Ps = 1 0 4 8  -> Save cursor as in DECSC.  (This may be disabled by the titeInhibit resource).
  uint8_t enable_alt_scree_and_save_cursor:2;   // Ps = 1 0 4 9  -> Save cursor as in DECSC and use Alternate Screen Buffer, clearing it first.  (This may be disabled by the titeInhibit resource).  This combines the effects of the 1047  and 1048  modes.  Use this with terminfo-based applications rather than the 47 mode.
  uint8_t _term_fnkey_mode:2;                   // Ps = 1 0 5 0  -> Set terminfo                       /termcap function-key mode.
  uint8_t _sun_fnkey_mode:2;                    // Ps = 1 0 5 1  -> Set Sun function-key mode.
  uint8_t _hp_fnkey_mode:2;                     // Ps = 1 0 5 2  -> Set HP function-key mode.
  uint8_t _sco_fnkey_mode:2;                    // Ps = 1 0 5 3  -> Set SCO function-key mode.
  uint8_t _legacy_keyboard:2;                   // Ps = 1 0 6 0  -> Set legacy keyboard emulation (X11R6).
  uint8_t _vt220_keyboard:2;                    // Ps = 1 0 6 1  -> Set VT220 keyboard emulation.
  uint8_t enable_bracketed_paste_mode:2;        // Ps = 2 0 0 4  -> Set bracketed paste mode.
} ANSI_Feature_Cmd;



//struct {
//  uint8_t alt_buffer:1;        // ?1049h
//  uint8_t origin_mode:1;       // ?6h
//  uint8_t wraparound:1;        // ?7h
//  uint8_t cursor_keys_app:1;   // ?1h
//  uint8_t keypad_app:1;        // ?66h
//  uint8_t mouse_tracking:1;    // ?1000h+
//  uint8_t focus_events:1;      // ?1004h
//  uint8_t bracketed_paste:1;   // ?2004h
//} ANSI_Mode_Cmd;
//struct {
//  uint8_t clear_screen:1;      // ESC[<n>J
//  uint8_t clear_screen_mode:2; // N in ESC[<n>J default 0
//  uint8_t clear_line  :1;      // ESC[<n>K
//  uint8_t clear_line_mode :2;  // N in ESC[<n>K default 0
//  uint8_t soft_reset  :1;      // ESC[!p
//  uint8_t save_cursor: 1;      // CSIs
//  uint8_t restore_cursor: 1;   // CSIu
//  uint16_t scroll_up;      // ESC[<n>S
//  uint16_t scroll_down;    // ESC[<n>T
//} ANSI_Action_Cmd;
