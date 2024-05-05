#pragma once

#include <string>
#include <vector>

typedef struct conn {
    std::string url;
    std::string para;
    std::vector<std::string> headers;
    std::string method;
    std::string response;
    std::string status;
    bool debug;
} conn_t;