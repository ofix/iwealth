#ifndef MACRO_H
#define MACRO_H

#define DELETE_RESOURCE(pointer) \
    if (pointer) {               \
        delete pointer;          \
        pointer = nullptr;       \
    }

#endif