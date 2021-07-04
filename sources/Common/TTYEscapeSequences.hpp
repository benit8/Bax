/*
** Bax, 2021
** Benoît Lormeau <blormeau@outlook.com>
** Terminal escape sequences, mostly colors
*/

#pragma once

// -----------------------------------------------------------------------------

#define TTY_ESC "\x1B"
#define TTY_CSI TTY_ESC "["

// -----------------------------------------------------------------------------

#define TTY_RESET        TTY_CSI "0m"
#define TTY_BOLD_ON      TTY_CSI "1m"
#define TTY_FAINT_ON     TTY_CSI "2m"
#define TTY_ITALIC_ON    TTY_CSI "3m"
#define TTY_UNDERLINE_ON TTY_CSI "4m"
#define TTY_BLINK_ON     TTY_CSI "5m"
#define TTY_REVERSE_ON   TTY_CSI "7m"
#define TTY_STRIKE_ON    TTY_CSI "9m"

#define TTY_BOLD_OFF      TTY_CSI "22m"
#define TTY_FAINT_OFF     TTY_CSI "22m"
#define TTY_ITALIC_OFF    TTY_CSI "23m"
#define TTY_UNDERLINE_OFF TTY_CSI "24m"
#define TTY_BLINK_OFF     TTY_CSI "25m"
#define TTY_REVERSE_OFF   TTY_CSI "27m"
#define TTY_STRIKE_OFF    TTY_CSI "29m"

#define TTY_FG_BLACK      TTY_CSI "30m"
#define TTY_FG_RED        TTY_CSI "31m"
#define TTY_FG_GREEN      TTY_CSI "32m"
#define TTY_FG_YELLOW     TTY_CSI "33m"
#define TTY_FG_BLUE       TTY_CSI "34m"
#define TTY_FG_MAGENTA    TTY_CSI "35m"
#define TTY_FG_CYAN       TTY_CSI "36m"
#define TTY_FG_WHITE      TTY_CSI "37m"
#define TTY_FG_256(n)     TTY_CSI "38;5;" #n "m"
#define TTY_FG_RGB(r,g,b) TTY_CSI "38;2;" #r ";" #g ";" #b "m"
#define TTY_FG_DEFAULT    TTY_CSI "39m"

#define TTY_BG_BLACK      TTY_CSI "40m"
#define TTY_BG_RED        TTY_CSI "41m"
#define TTY_BG_GREEN      TTY_CSI "42m"
#define TTY_BG_YELLOW     TTY_CSI "43m"
#define TTY_BG_BLUE       TTY_CSI "44m"
#define TTY_BG_MAGENTA    TTY_CSI "45m"
#define TTY_BG_CYAN       TTY_CSI "46m"
#define TTY_BG_WHITE      TTY_CSI "47m"
#define TTY_BG_256(n)     TTY_CSI "48;5;" #n "m"
#define TTY_BG_RGB(r,g,b) TTY_CSI "48;2;" #r ";" #g ";" #b "m"
#define TTY_BG_DEFAULT    TTY_CSI "49m"

#define TTY_FRAMED_ON     TTY_CSI "51m"
#define TTY_CIRCLED_ON    TTY_CSI "52m"
#define TTY_OVERLINED_ON  TTY_CSI "53m"
#define TTY_FRAMED_OFF    TTY_CSI "54m"
#define TTY_CIRCLED_OFF   TTY_CSI "54m"
#define TTY_OVERLINED_OFF TTY_CSI "55m"

// Underline color
#define TTY_UDL_256(n)     TTY_CSI "58;5;" #n "m"
#define TTY_UDL_RGB(r,g,b) TTY_CSI "58;2;" #r ";" #g ";" #b "m"
#define TTY_UDL_DEFAULT    TTY_CSI "59m"

#define TTY_FG_BRIGHT_BLACK   TTY_CSI "90m"
#define TTY_FG_BRIGHT_RED     TTY_CSI "91m"
#define TTY_FG_BRIGHT_GREEN   TTY_CSI "92m"
#define TTY_FG_BRIGHT_YELLOW  TTY_CSI "93m"
#define TTY_FG_BRIGHT_BLUE    TTY_CSI "94m"
#define TTY_FG_BRIGHT_MAGENTA TTY_CSI "95m"
#define TTY_FG_BRIGHT_CYAN    TTY_CSI "96m"
#define TTY_FG_BRIGHT_WHITE   TTY_CSI "97m"

#define TTY_BG_BRIGHT_BLACK   TTY_CSI "100m"
#define TTY_BG_BRIGHT_RED     TTY_CSI "101m"
#define TTY_BG_BRIGHT_GREEN   TTY_CSI "102m"
#define TTY_BG_BRIGHT_YELLOW  TTY_CSI "103m"
#define TTY_BG_BRIGHT_BLUE    TTY_CSI "104m"
#define TTY_BG_BRIGHT_MAGENTA TTY_CSI "105m"
#define TTY_BG_BRIGHT_CYAN    TTY_CSI "106m"
#define TTY_BG_BRIGHT_WHITE   TTY_CSI "107m"

// -----------------------------------------------------------------------------

#define TTY_BOLD(str)      TTY_BOLD_ON      str TTY_BOLD_OFF
#define TTY_FAINT(str)     TTY_FAINT_ON     str TTY_FAINT_OFF
#define TTY_ITALIC(str)    TTY_ITALIC_ON    str TTY_ITALIC_OFF
#define TTY_UNDERLINE(str) TTY_UNDERLINE_ON str TTY_UNDERLINE_OFF
#define TTY_BLINK(str)     TTY_BLINK_ON     str TTY_BLINK_OFF
#define TTY_REVERSE(str)   TTY_REVERSE_ON   str TTY_REVERSE_OFF
#define TTY_STRIKE(str)    TTY_STRIKE_ON    str TTY_STRIKE_OFF

#define TTY_BLACK(str)      TTY_FG_BLACK      str TTY_FG_DEFAULT
#define TTY_RED(str)        TTY_FG_RED        str TTY_FG_DEFAULT
#define TTY_GREEN(str)      TTY_FG_GREEN      str TTY_FG_DEFAULT
#define TTY_YELLOW(str)     TTY_FG_YELLOW     str TTY_FG_DEFAULT
#define TTY_BLUE(str)       TTY_FG_BLUE       str TTY_FG_DEFAULT
#define TTY_MAGENTA(str)    TTY_FG_MAGENTA    str TTY_FG_DEFAULT
#define TTY_CYAN(str)       TTY_FG_CYAN       str TTY_FG_DEFAULT
#define TTY_WHITE(str)      TTY_FG_WHITE      str TTY_FG_DEFAULT
#define TTY_256(n, str)     TTY_FG_256(n)     str TTY_FG_DEFAULT
#define TTY_RGB(r,g,b, str) TTY_FG_RGB(r,g,b) str TTY_FG_DEFAULT

#define TTY_BRIGHT_BLACK(str)   TTY_FG_BRIGHT_BLACK   str TTY_FG_DEFAULT
#define TTY_BRIGHT_RED(str)     TTY_FG_BRIGHT_RED     str TTY_FG_DEFAULT
#define TTY_BRIGHT_GREEN(str)   TTY_FG_BRIGHT_GREEN   str TTY_FG_DEFAULT
#define TTY_BRIGHT_YELLOW(str)  TTY_FG_BRIGHT_YELLOW  str TTY_FG_DEFAULT
#define TTY_BRIGHT_BLUE(str)    TTY_FG_BRIGHT_BLUE    str TTY_FG_DEFAULT
#define TTY_BRIGHT_MAGENTA(str) TTY_FG_BRIGHT_MAGENTA str TTY_FG_DEFAULT
#define TTY_BRIGHT_CYAN(str)    TTY_FG_BRIGHT_CYAN    str TTY_FG_DEFAULT
#define TTY_BRIGHT_WHITE(str)   TTY_FG_BRIGHT_WHITE   str TTY_FG_DEFAULT

#define TTY_ON_BLACK(str)      TTY_BG_BLACK      str TTY_BG_DEFAULT
#define TTY_ON_RED(str)        TTY_BG_RED        str TTY_BG_DEFAULT
#define TTY_ON_GREEN(str)      TTY_BG_GREEN      str TTY_BG_DEFAULT
#define TTY_ON_YELLOW(str)     TTY_BG_YELLOW     str TTY_BG_DEFAULT
#define TTY_ON_BLUE(str)       TTY_BG_BLUE       str TTY_BG_DEFAULT
#define TTY_ON_MAGENTA(str)    TTY_BG_MAGENTA    str TTY_BG_DEFAULT
#define TTY_ON_CYAN(str)       TTY_BG_CYAN       str TTY_BG_DEFAULT
#define TTY_ON_WHITE(str)      TTY_BG_WHITE      str TTY_BG_DEFAULT
#define TTY_ON_256(n, str)     TTY_BG_256(n)     str TTY_BG_DEFAULT
#define TTY_ON_RGB(r,g,b, str) TTY_BG_RGB(r,g,b) str TTY_BG_DEFAULT

#define TTY_ON_BRIGHT_BLACK(str)   TTY_BG_BRIGHT_BLACK   str TTY_BG_DEFAULT
#define TTY_ON_BRIGHT_RED(str)     TTY_BG_BRIGHT_RED     str TTY_BG_DEFAULT
#define TTY_ON_BRIGHT_GREEN(str)   TTY_BG_BRIGHT_GREEN   str TTY_BG_DEFAULT
#define TTY_ON_BRIGHT_YELLOW(str)  TTY_BG_BRIGHT_YELLOW  str TTY_BG_DEFAULT
#define TTY_ON_BRIGHT_BLUE(str)    TTY_BG_BRIGHT_BLUE    str TTY_BG_DEFAULT
#define TTY_ON_BRIGHT_MAGENTA(str) TTY_BG_BRIGHT_MAGENTA str TTY_BG_DEFAULT
#define TTY_ON_BRIGHT_CYAN(str)    TTY_BG_BRIGHT_CYAN    str TTY_BG_DEFAULT
#define TTY_ON_BRIGHT_WHITE(str)   TTY_BG_BRIGHT_WHITE   str TTY_BG_DEFAULT
