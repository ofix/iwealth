#pragma once

#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include "../curl/curl.h"
#include "conn.h"

#define BUF_SIZE 1024
#define HTTPS_TIME_OUT_MIN 15
#define HTTPS_TIME_OUT_MAX 20
#define gRequest Request::Instance()

class Request {
   public:
    static Request* Instance();
    virtual ~Request();
    static std::string Get(std::string url);
    static std::string Get(conn_t* conn);
    static std::string Post(conn_t* conn);
    static std::string Put(conn_t* conn);
    static std::string Patch(conn_t* conn);
    static std::string Delete(conn_t* conn);

   private:
    static inner_conn_t* CurlInit(conn_t* conn);
    static void SetRequestHeader(conn_t* conn, inner_conn_t* inner_conn);
    static void SetCommonOptions(conn_t* conn, inner_conn_t* inner_conn);
    static std::string DoRequest(conn_t* conn, inner_conn_t* inner_conn);
    static size_t CurlOnResponseBodyRecv(void* ptr,
                                         size_t size,
                                         size_t nmemb,
                                         void* data);
    static size_t CurlOnResponseHeaderRecv(void* ptr,
                                           size_t size,
                                           size_t nmemb,
                                           void* data);
    static void CurlClose(inner_conn_t* inner_conn);

    Request();
    Request(const Request&){};
    Request& operator=(const Request&) { return *this; };
    static Request* m_pThis;
};

class MultiRequest {
    CURL* handlers[8];  // 一次最多8个并发请求
    CURLM* multi_handler;
    std::thread thread_handler;
    std::vector<conn_t> conn_list;  // 请求列表
    int still_running = 1;
    int i;
    CURLMsg* msg;  /* for picking up messages with the transfer status */
    int msgs_left; /* how many messages are left */
};