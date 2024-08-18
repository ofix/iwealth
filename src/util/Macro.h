#ifndef UTIL_MACRO_H
#define UTIL_MACRO_H

#define DELETE_RESOURCE(ptr) \
    if (ptr) {               \
        delete (ptr);        \
        (ptr) = NULL;        \
    }

#endif