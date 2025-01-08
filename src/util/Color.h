#ifndef COLOR_H
#define COLOR_H

#define RST "\033[0m"
#define FRED "\033[1;31m"
#define FGREEN "\033[1;32m"
#define FYELLOW "\033[1;33m"
#define FBLUE "\033[1;34m"
#define FMAGENTA "\033[1;35m"
#define FCYAN "\033[1;36m"
#define FWHITE "\033[1;37m"

#define RED(x) FRED << x << RST
#define GREEN(x) FGREEN << x << RST
#define YELLOW(x) FYELLOW << x << RST
#define BLUE(x) FBLUE << x << RST
#define MAGENTA(x) FMAGENTA << x << RST
#define CYAN(x) FCYAN << x << RST
#define WHITE(x) FWHITE << x << RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDERLINE(x) "\x1B[4m" x RST

#define GREEN_COLOR wxColor(84, 255, 255)
#define RED_COLOR wxColor(255, 0, 0)
#define NORMAL_COLOR wxColor(215, 215, 215)

#define INDICATOR_COLOR_1 wxColor(230, 230, 230)
#define INDICATOR_COLOR_2 wxColor(255, 255, 0)
#define INDICATOR_COLOR_3 wxColor(255, 0, 255)

#define KLINE_PANEL_BACKGROUND_COLOR wxColor(25, 25, 25)
#define KLINE_PANEL_BORDER_COLOR wxColor(50, 51, 55)

#endif