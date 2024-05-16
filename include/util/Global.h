#pragma once
#include <array>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>
#include "Color.h"

#ifdef _WIN32
#define DIRECTORY_SEPARATOR "\\"
#else
#define DIRECTORY_SEPARATOR "/"
#endif

typedef struct _cell_size {
    uint16_t nrows;
    uint16_t ncols;
} cell_size;

std::string exec(const char* cmd);
std::string now();
std::vector<std::string> split(const std::string& str, const char& separator = '\n');
bool end_with(const std::string& origin, const std::string& target);
void trim(std::string& str);
void ltrim(std::string& str);
void rtrim(std::string& str);
std::string to_upper_case(const std::string& str);
std::string to_lower_case(const std::string& str);
int substring_count(const std::string& str, const std::string& sub);
void to_upper_case(std::string& str);
void to_lower_case(std::string& str);
std::string trim_copy(std::string str);
std::string ltrim_copy(std::string str);
std::string rtrim_copy(std::string str);
void padding_left(std::string& str, uint16_t size, const char needle = ' ');
void padding_right(std::string& str, uint16_t size, const char needle = ' ');
// 获取当前可执行文件所在目录
std::string current_execute_path();

std::string repeat_chars(char ch, int count);
void fill_chars(std::string& str,
                const char needle,
                size_t width,
                const std::string direction = "left");

// 16进制转10进制模板函数
template <typename T>
T hex_to_dec(const std::string& hex) {
    std::string h = hex;
    if (h[0] == '0' && (h[1] == 'x' || h[1] == 'X')) {
        h = hex.substr(2);
    }
    T dec = 0;
    T base = 1;
    T val = 0;
    for (int i = h.length() - 1; i >= 0; i--) {
        if (h[i] >= '0' && h[i] <= '9') {
            val = static_cast<T>(h[i] - '0');
        } else if (h[i] >= 'a' && h[i] <= 'f') {
            val = static_cast<T>(h[i] - 'a') + 10;
        } else if (h[i] >= 'A' && h[i] <= 'F') {
            val = static_cast<T>(h[i] - 'A') + 10;
        }
        dec += val * base;
        base *= 16;
    }
    return dec;
}

// 10进制转16进制模板函数
template <typename T>
std::string dec_to_hex(T dec) {
    std::string hex = "";
    int val;
    if (dec == 0) {
        return "0x00";
    }
    while (dec > 0) {
        val = dec % 16;
        if (val >= 10 && val <= 15) {
            hex = static_cast<char>(val + 55) + hex;
        } else {
            hex = static_cast<char>(val + 48) + hex;
        }
        dec /= 16;
    }
    if (hex.length() % 2 != 0) {
        hex = '0' + hex;
    }
    return "0x" + hex;
}

template <typename T>
std::string to_string(T dec) {
    std::string result = "";
    int val;
    if (dec == 0) {
        return "0";
    }
    while (dec > 0) {
        val = dec % 10;
        result = static_cast<char>(val + 48) + result;
        dec /= 10;
    }
    return result;
}
