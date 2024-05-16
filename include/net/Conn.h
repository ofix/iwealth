#pragma once

#include <functional>
#include <string>
#include <vector>

struct upload_info_t {
    FILE* file_src;
    long file_size;
    std::string file_name;
    std::string file_path;
};

struct conn_t {
    std::string url;
    std::string method;
    std::string payload;
    std::vector<std::string> headers;
    std::string status;
    upload_info_t* upload_info;
    int http_code;
    int retry = 1;       // 重试次数
    int timeout = 5000;  // 请求超时时间
    std::string basic_auth_name;
    std::string basic_auth_pwd;
    CURL* easy_curl;                             // HTTP(s)请求句柄
    struct curl_slist* curl_header_list;         // CURL 依赖头
    curl_off_t queue_time;                       // 请求排队时间
    curl_off_t total_time;                       // 请求响应总时间
    std::string response;                        // 返回响应数据
    std::function<void(std::string&)> callback;  // HTTP(s)回调响应处理函数
    bool debug = false;
};