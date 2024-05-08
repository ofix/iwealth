#pragma once

#include <functional>
#include <string>
#include <vector>

typedef struct upload_info {
    FILE* file_src;
    long file_size;
    std::string file_name;
    std::string file_path;
} upload_info_t;

typedef struct conn {
    std::string url;
    std::string method;
    std::string payload;
    std::vector<std::string> headers;
    std::string response;
    std::string status;
    upload_info_t* upload_info;
    int http_code;
    int retry = 1;       // 重试次数
    int timeout = 5000;  // 请求超时时间
    std::string basic_auth_name;
    std::string basic_auth_pwd;
    std::function<int(std::string&)> callback;
    bool debug = false;
} conn_t;

typedef struct inner_conn {
    CURL* easy_curl;
    struct curl_slist* curl_header_list;
    std::string recv_buf;
    size_t recv_size;
} inner_conn_t;