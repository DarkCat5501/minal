#!/bin/bash
# ANSI Escape Sequence Test Suite
# Generated from ctlseqs.txt

ESC=$'\033'
CSI="${ESC}["
OSC="${ESC}]"
ST="${ESC}\\"
BEL=$'\a'

echo "=== ANSI Escape Sequence Test Suite ==="
echo ""

# -----------------------------------------------------------------------------
# 1. Single-character functions
# -----------------------------------------------------------------------------
echo "=== 1. Single-character Functions ==="
echo -e "BEL: ${BEL}"
echo -en "BS: ${CSI}1D"  # Backspace
echo -en "CR: ${CR}"
echo -en "FF: ${FF}"
echo -en "LF: ${LF}"
echo -en "SI: ${SI}"    # Shift In (to G0)
echo -en "SO: ${SO}"    # Shift Out (to G1)
echo -en "TAB: ${TAB}"
echo -en "VT: ${VT}"
echo ""

# -----------------------------------------------------------------------------
# 2. ESC Controls - Character Sets (ESC ( C, ESC ) C, ESC * C, ESC + C)
# -----------------------------------------------------------------------------
echo "=== 2. Character Set Designation ==="
echo "G0 (94-character sets):"
echo -en "DEC Special: ${ESC}(0"
echo -en "UK: ${ESC}(A"
echo -en "US ASCII: ${ESC}(B"
echo -en "Dutch: ${ESC}(4"
echo -en "Finnish: ${ESC}(C"
echo -en "French: ${ESC}(R"
echo -en "French Canadian: ${ESC}(Q"
echo -en "German: ${ESC}(K"
echo -en "Italian: ${ESC}(Y"
echo -en "Norwegian/Danish: ${ESC}(E"
echo -en "Spanish: ${ESC}(Z"
echo -en "Swedish: ${ESC}(H"
echo -en "Swiss: ${ESC}("
echo "G1:"
echo -en "DEC Special: ${ESC})0"
echo -en "UK: ${ESC})A"
echo -en "US ASCII: ${ESC})B"
echo "G2:"
echo -en "DEC Special: ${ESC}*0"
echo "G3:"
echo -en "DEC Special: ${ESC}+0"
echo ""

# -----------------------------------------------------------------------------
# 3. VT100 Special Sequences (ESC 7, ESC 8, ESC =, ESC >, ESC c, etc.)
# -----------------------------------------------------------------------------
echo "=== 3. VT100 Special Sequences ==="
echo -e "Save Cursor (DECSC): ${ESC}7"
echo -e "Restore Cursor (DECRC): ${ESC}8"
echo -e "Application Keypad (DECKPAM): ${ESC}="
echo -e "Normal Keypad (DECKPNM): ${ESC}>"
echo -e "Full Reset (RIS): ${ESC}c"
echo -e "Memory Lock: ${ESC}l"
echo -e "Memory Unlock: ${ESC}m"
echo -e "Invoke G2 (LS2): ${ESC}n"
echo -e "Invoke G3 (LS3): ${ESC}o"
echo -e "Invoke G3 as GR (LS3R): ${ESC}|"
echo -e "Invoke G2 as GR (LS2R): ${ESC}}"
echo -e "Invoke G1 as GR (LS1R): ${ESC}~"
echo -e "Back Index (DECBI): ${ESC}6"
echo -e "Forward Index (DECFI): ${ESC}9"
echo ""

# -----------------------------------------------------------------------------
# 4. DEC Line Drawing (ESC # 3-8)
# -----------------------------------------------------------------------------
echo "=== 4. DEC Line Drawing ==="
echo -e "Double-height top: ${ESC}#3"
echo -e "Double-height bottom: ${ESC}#4"
echo -e "Single-width: ${ESC}#5"
echo -e "Double-width: ${ESC}#6"
echo -e "Screen Alignment Test: ${ESC}#8"
echo ""

# -----------------------------------------------------------------------------
# 5. CSI - Cursor Movement
# -----------------------------------------------------------------------------
echo "=== 5. Cursor Movement (CSI Ps {A,B,C,D,E,F,G,H}) ==="
echo -e "Cursor Up (CUU): ${CSI}1A ${CSI}5A ${CSI}10A"
echo -e "Cursor Down (CUD): ${CSI}1B ${CSI}5B ${CSI}10B"
echo -e "Cursor Forward (CUF): ${CSI}1C ${CSI}5C ${CSI}10C"
echo -e "Cursor Backward (CUB): ${CSI}1D ${CSI}5D ${CSI}10D"
echo -e "Cursor Next Line (CNL): ${CSI}1E ${CSI}5E"
echo -e "Cursor Preceding Line (CPL): ${CSI}1F ${CSI}5F"
echo -e "Cursor Position Absolute (CHA): ${CSI}10G ${CSI}20G"
echo -e "Cursor Position (CUP): ${CSI}1;1H ${CSI}5;10H ${CSI};10H ${CSI}10;H"
echo -e "Cursor Backward Tab (CBT): ${CSI}1Z ${CSI}3Z"
echo -e "Cursor Forward Tab (CHT): ${CSI}1I ${CSI}5I"
echo -e "Horizontal/Vertical Position (HVP): ${CSI}1;1f ${CSI}5;10f"
echo -e "Line Position Absolute (VPA): ${CSI}10d ${CSI}20d"
echo -e "Line Position Relative (VPR): ${CSI}10e ${CSI}20e"
echo -e "Character Position Absolute (HPA): ${CSI}10\`"
echo -e "Character Position Relative (HPR): ${CSI}10a"
echo ""

# -----------------------------------------------------------------------------
# 6. CSI - Erase (ED/EL)
# -----------------------------------------------------------------------------
echo "=== 6. Erase Functions (CSI Ps J / CSI Ps K) ==="
echo "Erase in Display (ED):"
echo -e "  Erase Below (0): ${CSI}J ${CSI}0J"
echo -e "  Erase Above (1): ${CSI}1J"
echo -e "  Erase All (2): ${CSI}2J"
echo -e "  Erase Saved Lines (3): ${CSI}3J"
echo "DECSED (CSI ? Ps J):"
echo -e "  Selective Erase Below: ${CSI}?J ${CSI}?0J"
echo -e "  Selective Erase Above: ${CSI}?1J"
echo -e "  Selective Erase All: ${CSI}?2J"
echo "Erase in Line (EL):"
echo -e "  Erase to Right (0): ${CSI}K ${CSI}0K"
echo -e "  Erase to Left (1): ${CSI}1K"
echo -e "  Erase All (2): ${CSI}2K"
echo "DECSEL (CSI ? Ps K):"
echo -e "  Selective Erase to Right: ${CSI}?K ${CSI}?0K"
echo -e "  Selective Erase to Left: ${CSI}?1K"
echo -e "  Selective Erase All: ${CSI}?2K"
echo ""

# -----------------------------------------------------------------------------
# 7. CSI - Insert/Delete Lines/Chars
# -----------------------------------------------------------------------------
echo "=== 7. Insert/Delete Functions ==="
echo -e "Insert Character (ICH): ${CSI}10@ ${CSI}5@"
echo -e "Delete Character (DCH): ${CSI}10P ${CSI}5P"
echo -e "Insert Line (IL): ${CSI}10L ${CSI}5L"
echo -e "Delete Line (DL): ${CSI}10M ${CSI}5M"
echo -e "Erase Character (ECH): ${CSI}10X ${CSI}5X"
echo ""

# -----------------------------------------------------------------------------
# 8. CSI - Scroll
# -----------------------------------------------------------------------------
echo "=== 8. Scroll Functions ==="
echo -e "Scroll Up (SU): ${CSI}1S ${CSI}5S ${CSI}10S"
echo -e "Scroll Down (SD): ${CSI}1T ${CSI}5T ${CSI}10T"
echo ""

# -----------------------------------------------------------------------------
# 9. CSI - Tab Functions
# -----------------------------------------------------------------------------
echo "=== 9. Tab Functions ==="
echo -e "Tab Clear (TBC): ${CSI}3g ${CSI}0g"
echo "  Clear Current Column (0): ${CSI}0g"
echo "  Clear All (3): ${CSI}3g"
echo ""

# -----------------------------------------------------------------------------
# 10. CSI - Device Status Report (DSR)
# -----------------------------------------------------------------------------
echo "=== 10. Device Status Report (DSR) ==="
echo -e "Status Report: ${CSI}5n"
echo -e "Cursor Position Report: ${CSI}6n"
echo "DEC Private DSR:"
echo -e "  Cursor Position: ${CSI}?6n"
echo -e "  Printer Status: ${CSI}?15n"
echo -e "  UDK Status: ${CSI}?25n"
echo -e "  Keyboard Status: ${CSI}?26n"
echo -e "  Locator Status: ${CSI}?53n"
echo -e "  Data Integrity: ${CSI}?70n"
echo -e "  Multi-session: ${CSI}?83n"
echo ""

# -----------------------------------------------------------------------------
# 11. CSI - Set/Reset Modes (SM/RM)
# -----------------------------------------------------------------------------
echo "=== 11. Set/Reset Modes (SM/RM) ==="
echo "Set Mode (CSI Ps h):"
echo -e "  Keyboard Action (2): ${CSI}2h"
echo -e "  Insert Mode (4): ${CSI}4h"
echo -e "  Send/Receive (12): ${CSI}12h"
echo -e "  Automatic Newline (20): ${CSI}20h"
echo "Reset Mode (CSI Ps l):"
echo -e "  Keyboard Action (2): ${CSI}2l"
echo -e "  Replace Mode (4): ${CSI}4l"
echo -e "  Send/Receive (12): ${CSI}12l"
echo -e "  Normal Linefeed (20): ${CSI}20l"
echo ""

# -----------------------------------------------------------------------------
# 12. CSI - DEC Private Modes (DECSET/DECRST)
# -----------------------------------------------------------------------------
echo "=== 12. DEC Private Modes (DECSET/DECRST) ==="
echo "DECSET (CSI ? Ps h):"
echo -e "  Application Cursor Keys (1): ${CSI}?1h"
echo -e "  USASCII/VT100 mode (2): ${CSI}?2h"
echo -e "  132 Columns (3): ${CSI}?3h"
echo -e "  Smooth Scroll (4): ${CSI}?4h"
echo -e "  Reverse Video (5): ${CSI}?5h"
echo -e "  Origin Mode (6): ${CSI}?6h"
echo -e "  Wraparound (7): ${CSI}?7h"
echo -e "  Auto-repeat Keys (8): ${CSI}?8h"
echo -e "  X10 Mouse (9): ${CSI}?9h"
echo -e "  Show Toolbar (10): ${CSI}?10h"
echo -e "  Blinking Cursor (12): ${CSI}?12h"
echo -e "  Print Form Feed (18): ${CSI}?18h"
echo -e "  Print Extent (19): ${CSI}?19h"
echo -e "  Show Cursor (25): ${CSI}?25h"
echo -e "  Show Scrollbar (30): ${CSI}?30h"
echo -e "  Enable Font-shifting (35): ${CSI}?35h"
echo -e "  Allow 80->132 (40): ${CSI}?40h"
echo -e "  More Fix (41): ${CSI}?41h"
echo -e "  NRCM (42): ${CSI}?42h"
echo -e "  Margin Bell (44): ${CSI}?44h"
echo -e "  Reverse-wraparound (45): ${CSI}?45h"
echo -e "  Logging (46): ${CSI}?46h"
echo -e "  Alternate Screen (47): ${CSI}?47h"
echo -e "  Application Keypad (66): ${CSI}?66h"
echo -e "  Backarrow is Backspace (67): ${CSI}?67h"
echo -e "  Enable LRMM (69): ${CSI}?69h"
echo -e "  No Clear on DECCOLM (95): ${CSI}?95h"
echo -e "  VT200 Mouse (1000): ${CSI}?1000h"
echo -e "  Hilite Mouse (1001): ${CSI}?1001h"
echo -e "  Cell Motion Mouse (1002): ${CSI}?1002h"
echo -e "  All Motion Mouse (1003): ${CSI}?1003h"
echo -e "  Focus Events (1004): ${CSI}?1004h"
echo -e "  UTF8 Mouse (1005): ${CSI}?1005h"
echo -e "  SGR Mouse (1006): ${CSI}?1006h"
echo -e "  Alternate Scroll (1007): ${CSI}?1007h"
echo -e "  Autoscroll on Output (1010): ${CSI}?1010h"
echo -e "  urxvt Mouse (1015): ${CSI}?1015h"
echo -e "  Autoscroll on Keypress (1011): ${CSI}?1011h"
echo -e "  8Bit Input (1034): ${CSI}?1034h"
echo -e "  NumLock (1035): ${CSI}?1035h"
echo -e "  MetaSendsEscape (1036): ${CSI}?1036h"
echo -e "  Delete sends DEL (1037): ${CSI}?1037h"
echo -e "  AltSendsEscape (1039): ${CSI}?1039h"
echo -e "  Keep Selection (1040): ${CSI}?1040h"
echo -e "  Select to Clipboard (1041): ${CSI}?1041h"
echo -e "  Bell is Urgent (1042): ${CSI}?1042h"
echo -e "  Pop on Bell (1043): ${CSI}?1043h"
echo -e "  Alternate Screen (1047): ${CSI}?1047h"
echo -e "  Save Cursor (1048): ${CSI}?1048h"
echo -e "  Alt Screen + Save Cursor (1049): ${CSI}?1049h"
echo -e "  Bracketed Paste (2004): ${CSI}?2004h"
echo ""
echo "DECRST (CSI ? Ps l):"
echo -e "  Normal Cursor Keys (1): ${CSI}?1l"
echo -e "  VT52 Mode (2): ${CSI}?2l"
echo -e "  80 Columns (3): ${CSI}?3l"
echo -e "  Jump Scroll (4): ${CSI}?4l"
echo -e "  Normal Video (5): ${CSI}?5l"
echo -e "  Normal Cursor Mode (6): ${CSI}?6l"
echo -e "  No Wraparound (7): ${CSI}?7l"
echo -e "  No Auto-repeat (8): ${CSI}?8l"
echo -e "  No X10 Mouse (9): ${CSI}?9l"
echo -e "  Hide Cursor (25): ${CSI}?25l"
echo -e "  Normal Screen (47): ${CSI}?47l"
echo -e "  Normal Screen + Clear (1047): ${CSI}?1047l"
echo -e "  Restore Cursor (1048): ${CSI}?1048l"
echo -e "  Normal Screen + Restore (1049): ${CSI}?1049l"
echo ""

# -----------------------------------------------------------------------------
# 13. CSI - SGR (Select Graphic Rendition)
# -----------------------------------------------------------------------------
echo "=== 13. Select Graphic Rendition (SGR) ==="
echo "Basic:"
echo -e "  Reset: ${CSI}0m"
echo -e "  Bold: ${CSI}1m"
echo -e "  Faint: ${CSI}2m"
echo -e "  Italic: ${CSI}3m"
echo -e "  Underline: ${CSI}4m"
echo -e "  Blink: ${CSI}5m"
echo -e "  Reverse: ${CSI}7m"
echo -e "  Invisible: ${CSI}8m"
echo -e "  Strikethrough: ${CSI}9m"
echo ""
echo "Cancel attributes:"
echo -e "  Normal (not bold/faint): ${CSI}22m"
echo -e "  Not italic: ${CSI}23m"
echo -e "  Not underlined: ${CSI}24m"
echo -e "  Steady (not blink): ${CSI}25m"
echo -e "  Positive (not reverse): ${CSI}27m"
echo -e "  Visible (not invisible): ${CSI}28m"
echo -e "  Not strikethrough: ${CSI}29m"
echo ""
echo "Underline styling (xterm):"
echo -e "  Underline: ${CSI}4m"
echo -e "  Double underline: ${CSI}4:2m"
echo -e "  Curly underline: ${CSI}4:3m"
echo -e "  Dotted underline: ${CSI}4:4m"
echo -e "  Dashed underline: ${CSI}4:5m"
echo ""
echo "Foreground colors (standard):"
echo -e "  Black: ${CSI}30m"
echo -e "  Red: ${CSI}31m"
echo -e "  Green: ${CSI}32m"
echo -e "  Yellow: ${CSI}33m"
echo -e "  Blue: ${CSI}34m"
echo -e "  Magenta: ${CSI}35m"
echo -e "  Cyan: ${CSI}36m"
echo -e "  White: ${CSI}37m"
echo -e "  Default: ${CSI}39m"
echo ""
echo "Background colors (standard):"
echo -e "  Black: ${CSI}40m"
echo -e "  Red: ${CSI}41m"
echo -e "  Green: ${CSI}42m"
echo -e "  Yellow: ${CSI}43m"
echo -e "  Blue: ${CSI}44m"
echo -e "  Magenta: ${CSI}45m"
echo -e "  Cyan: ${CSI}46m"
echo -e "  White: ${CSI}47m"
echo -e "  Default: ${CSI}49m"
echo ""
echo "Foreground colors (bright/256):"
echo -e "  Bright Black: ${CSI}90m"
echo -e "  Bright Red: ${CSI}91m"
echo -e "  Bright Green: ${CSI}92m"
echo -e "  Bright Yellow: ${CSI}93m"
echo -e "  Bright Blue: ${CSI}94m"
echo -e "  Bright Magenta: ${CSI}95m"
echo -e "  Bright Cyan: ${CSI}96m"
echo -e "  Bright White: ${CSI}97m"
echo ""
echo "Background colors (bright/256):"
echo -e "  Bright Black: ${CSI}100m"
echo -e "  Bright Red: ${CSI}101m"
echo -e "  Bright Green: ${CSI}102m"
echo -e "  Bright Yellow: ${CSI}103m"
echo -e "  Bright Blue: ${CSI}104m"
echo -e "  Bright Magenta: ${CSI}105m"
echo -e "  Bright Cyan: ${CSI}106m"
echo -e "  Bright White: ${CSI}107m"
echo ""
echo "256-color mode:"
echo -e "  FG 256-color: ${CSI}38:5:123m"
echo -e "  BG 256-color: ${CSI}48:5:123m"
echo ""
echo "Truecolor mode:"
echo -e "  FG Truecolor: ${CSI}38:2:255:128:0m"
echo -e "  BG Truecolor: ${CSI}48:2:255:128:0m"
echo ""
echo "Underline color (xterm):"
echo -e "  UL color 256: ${CSI}58:5:123m"
echo -e "  UL color Truecolor: ${CSI}58:2:255:128:0m"
echo -e "  UL color default: ${CSI}59m"
echo ""

# -----------------------------------------------------------------------------
# 14. CSI - Scroll Region (DECSTBM)
# -----------------------------------------------------------------------------
echo "=== 14. Scroll Region (DECSTBM) ==="
echo -e "Set scroll region: ${CSI}10;20r"
echo -e "Reset scroll region: ${CSI}r"
echo ""

# -----------------------------------------------------------------------------
# 15. CSI - Margins (DECSLRM)
# -----------------------------------------------------------------------------
echo "=== 15. Margins (DECSLRM) ==="
echo -e "Set margins: ${CSI}5;100s"
echo ""

# -----------------------------------------------------------------------------
# 16. CSI - Save/Restore Cursor (ANSI)
# -----------------------------------------------------------------------------
echo "=== 16. Save/Restore Cursor (ANSI) ==="
echo -e "Save Cursor (ANSI): ${CSI}s"
echo -e "Restore Cursor (ANSI): ${CSI}u"
echo "DEC Private:"
echo -e "  Save DEC Private: ${CSI}?1048s"
echo -e "  Restore DEC Private: ${CSI}?1048r"
echo ""

# -----------------------------------------------------------------------------
# 17. CSI - Window Manipulation
# -----------------------------------------------------------------------------
echo "=== 17. Window Manipulation (CSI Ps ; Ps ; Ps t) ==="
echo -e "De-iconify: ${CSI}1t"
echo -e "Iconify: ${CSI}2t"
echo -e "Move window: ${CSI}3;100;200t"
echo -e "Resize in pixels: ${CSI}4;400;300t"
echo -e "Raise: ${CSI}5t"
echo -e "Lower: ${CSI}6t"
echo -e "Refresh: ${CSI}7t"
echo -e "Resize in chars: ${CSI}8;25;80t"
echo -e "Maximize: ${CSI}9;1t"
echo -e "Unmaximize: ${CSI}9;0t"
echo -e "Fullscreen: ${CSI}10;2t"
echo -e "Report window state: ${CSI}11t"
echo -e "Report window position: ${CSI}13t"
echo -e "Report window size (pixels): ${CSI}14t"
echo -e "Report text area size (chars): ${CSI}18t"
echo -e "Report screen size (chars): ${CSI}19t"
echo -e "Report icon title: ${CSI}20t"
echo -e "Report window title: ${CSI}21t"
echo "Title stack:"
echo -e "  Save (0): ${CSI}22;0t"
echo -e "  Save icon (1): ${CSI}22;1t"
echo -e "  Save window (2): ${CSI}22;2t"
echo -e "  Restore (0): ${CSI}23;0t"
echo -e "  Restore icon (1): ${CSI}23;1t"
echo -e "  Restore window (2): ${CSI}23;2t"
echo ""

# -----------------------------------------------------------------------------
# 18. CSI - Misc
# -----------------------------------------------------------------------------
echo "=== 18. Misc CSI Sequences ==="
echo -e "Repeat (REP): ${CSI}5b"
echo -e "Soft Reset (DECSTR): ${CSI}!p"
echo -e "Full Reset (RIS): ${CSI}c"
echo -e "Load LEDs (DECLL): ${CSI}0q ${CSI}1q ${CSI}2q ${CSI}3q"
echo -e "Load LEDs off: ${CSI}21q ${CSI}22q ${CSI}23q"
echo -e "Cursor Style: ${CSI}0 q ${CSI}1 q ${CSI}2 q ${CSI}3 q ${CSI}4 q ${CSI}5 q ${CSI}6 q"
echo -e "Select Attr (DECSCA): ${CSI}\"p ${CSI}0\"p ${CSI}1\"p ${CSI}2\"p"
echo -e "Request Mode (DECRQM): ${CSI}\$p ${CSI}?\$p"
echo ""

# -----------------------------------------------------------------------------
# 19. CSI - Rectangle Operations
# -----------------------------------------------------------------------------
echo "=== 19. Rectangle Operations ==="
echo -e "Fill Rectangle (DECFRA): ${CSI}32;1;1;10;20\$x"
echo -e "Erase Rectangle (DECERA): ${CSI}1;1;10;20\$z"
echo -e "Change Attributes (DECCARA): ${CSI}1;1;10;20;1\$r"
echo -e "Reverse Attributes (DECRARA): ${CSI}1;1;10;20;1\$t"
echo -e "Copy Rectangle (DECCRA): ${CSI}1;1;10;20;1;1;1\$v"
echo -e "Selective Erase (DECSERA): ${CSI}1;1;10;20\$z"
echo -e "Request Checksum (DECRQCRA): ${CSI}1;1;10;20;1*\$y"
echo ""

# -----------------------------------------------------------------------------
# 20. CSI - Locator
# -----------------------------------------------------------------------------
echo "=== 20. Locator ==="
echo -e "Enable Locator (DECELR): ${CSI}0'z ${CSI}1'z ${CSI}2'z"
echo -e "Locator Events (DECSLE): ${CSI}0'{ ${CSI}1'{ ${CSI}2'{ ${CSI}3'{ ${CSI}4'{"
echo -e "Request Locator (DECRQLP): ${CSI}0'|"
echo ""

# -----------------------------------------------------------------------------
# 21. CSI - Media Copy
# -----------------------------------------------------------------------------
echo "=== 21. Media Copy (MC) ==="
echo -e "Print screen: ${CSI}0i"
echo -e "Print line: ${CSI}?0i"
echo -e "Turn off printer: ${CSI}4i"
echo -e "Turn on printer: ${CSI}5i"
echo -e "Print composed: ${CSI}?10i"
echo -e "Print all: ${CSI}?11i"
echo ""

# -----------------------------------------------------------------------------
# 22. OSC - Operating System Commands
# -----------------------------------------------------------------------------
echo "=== 22. Operating System Commands (OSC) ==="
echo "Window/Icon Title:"
echo -e "  Set title (0): ${OSC}0;Test Title${BEL}"
echo -e "  Set icon (1): ${OSC}1;Test Icon${BEL}"
echo -e "  Set window (2): ${OSC}2;Test Window${BEL}"
echo "Colors:"
echo -e "  Set color: ${OSC}4;0;rgb:ff/00/00${BEL}"
echo -e "  Set color (256): ${OSC}4;0;#ff0000${BEL}"
echo -e "  Reset color: ${OSC}104;0${BEL}"
echo -e "  Reset all colors: ${OSC}104${BEL}"
echo "Dynamic colors:"
echo -e "  FG (10): ${OSC}10;rgb:ff/00/00${BEL}"
echo -e "  BG (11): ${OSC}11;rgb:ff/00/00${BEL}"
echo -e "  Cursor (12): ${OSC}12;rgb:ff/00/00${BEL}"
echo -e "  Mouse FG (13): ${OSC}13;rgb:ff/00/00${BEL}"
echo -e "  Mouse BG (14): ${OSC}14;rgb:ff/00/00${BEL}"
echo -e "  Reset FG (110): ${OSC}110${BEL}"
echo -e "  Reset BG (111): ${OSC}111${BEL}"
echo -e "  Reset cursor (112): ${OSC}112${BEL}"
echo "Special:"
echo -e "  Font (50): ${OSC}50;monospace${BEL}"
echo -e "  Font index (50): ${OSC}50;#0${BEL}"
echo "Hyperlink (8):"
echo -e "  Start: ${OSC}8;http://example.com;Example${BEL}"
echo -e "  End: ${OSC}8;;${BEL}"
echo "Clipboard (52):"
echo -e "  Copy: ${OSC}52;c;dGVzdA==${BEL}"
echo -e "  Paste: ${OSC}52;p;?${BEL}"
echo ""

# -----------------------------------------------------------------------------
# 23. DCS - Device Control Strings
# -----------------------------------------------------------------------------
echo "=== 23. Device Control Strings (DCS) ==="
echo "User-defined Keys (DECUDK):"
echo -e "  ${DCS}0;0|${ST}"
echo "Request Status (DECRQSS):"
echo -e "  ${DCS}1\$q${ST}"
echo "Termcap/Terminfo:"
echo -e "  ${DCS}+p${ST}"
echo -e "  ${DCS}+q${ST}"
echo ""

# -----------------------------------------------------------------------------
# 24. C1 Control Characters (8-bit)
# -----------------------------------------------------------------------------
echo "=== 24. C1 Control Characters ==="
# These are 8-bit control characters (0x80-0x9F)
# Not all terminals support these; they're typically represented as 7-bit sequences
echo "Note: C1 characters are 8-bit (0x80-0x9F) and may not display correctly"
echo "  IND (0x84): \\x84"
echo "  NEL (0x85): \\x85"
echo "  HTS (0x88): \\x88"
echo "  RI (0x8D): \\x8D"
echo "  SS2 (0x8E): \\x8E"
echo "  SS3 (0x8F): \\x8F"
echo "  DCS (0x90): \\x90"
echo "  CSI (0x9B): \\x9B"
echo "  OSC (0x9D): \\x9D"
echo "  ST (0x9C): \\x9C"
echo ""

# -----------------------------------------------------------------------------
# 25. VT52 Mode
# -----------------------------------------------------------------------------
echo "=== 25. VT52 Mode ==="
echo "Note: VT52 sequences require switching to VT52 mode first"
echo -e "Cursor Up: ${ESC}A"
echo -e "Cursor Down: ${ESC}B"
echo -e "Cursor Right: ${ESC}C"
echo -e "Cursor Left: ${ESC}D"
echo -e "Enter Graphics: ${ESC}F"
echo -e "Exit Graphics: ${ESC}G"
echo -e "Home: ${ESC}H"
echo -e "Reverse Line Feed: ${ESC}I"
echo -e "Erase to End of Screen: ${ESC}J"
echo -e "Erase to End of Line: ${ESC}K"
echo -e "Cursor Position: ${ESC}Y\$row\$col"
echo -e "Identify: ${ESC}Z"
echo -e "Enter Alternate Keypad: ${ESC}="
echo -e "Exit Alternate Keypad: ${ESC}>"
echo -e "Exit VT52 Mode: ${ESC}<"
echo ""

# -----------------------------------------------------------------------------
# 26. xterm-specific Extensions
# -----------------------------------------------------------------------------
echo "=== 26. xterm-specific Extensions ==="
echo "Title modes (CSI > Ps; Ps T):"
echo -e "  Set hex title: ${CSI}>0;0T"
echo -e "  Query hex title: ${CSI}>1;0T"
echo "Modify keyboard (CSI > Ps; Ps m):"
echo -e "  modifyKeyboard: ${CSI}>0;1m"
echo -e "  modifyCursorKeys: ${CSI}>1;1m"
echo -e "  modifyFunctionKeys: ${CSI}>2;1m"
echo -e "  modifyOtherKeys: ${CSI}>4;1m"
echo "SGR Push/Pop (xterm):"
echo -e "  Push SGR: ${CSI}#{$"
echo -e "  Pop SGR: ${CSI}#{$"
echo "Color push/pop:"
echo -e "  Push colors: ${CSI}#{$"
echo -e "  Pop colors: ${CSI}#{$"
echo ""

# -----------------------------------------------------------------------------
# 27. Bracketed Paste Mode
# -----------------------------------------------------------------------------
echo "=== 27. Bracketed Paste Mode ==="
echo -e "Enable: ${CSI}?2004h"
echo -e "Disable: ${CSI}?2004l"
echo -e "Paste start: ${CSI}200~"
echo -e "Paste end: ${CSI}201~"
echo ""

# -----------------------------------------------------------------------------
# Summary
# -----------------------------------------------------------------------------
echo "=== Test Suite Complete ==="
echo "Total categories: 27"
echo ""
echo "Usage: Run this script and observe terminal behavior"
echo "Note: Some sequences may not work in all terminals"