#ifndef UTIL_MACRO_H
#define UTIL_MACRO_H

#define DELETE_RESOURCE(ptr) \
    if (ptr) {               \
        delete (ptr);        \
        (ptr) = NULL;        \
    }

#define KLINE_PANEL_TOP_CTRL_HEIGHT 20
#define KLINE_PANEL_PADDING 4
#define INDICATOR_CTRL_HEIGHT 140

#endif