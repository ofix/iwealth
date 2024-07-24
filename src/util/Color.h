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

#endif