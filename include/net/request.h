#pragma once

#include <thread>
#include "conn.h"
#include "curl/curl.h"

#define BUF_SIZE 1024
#define HTTPS_TIME_OUT_MIN 15
#define HTTPS_TIME_OUT_MAX 20

class Request {
   public:
    static char* doGet(conn_t* conn);
    static char* doPost(conn_t* conn);
    static char* doPut(conn_t* conn);
    static char* doPatch(conn_t* conn);
    static char* doDelete(conn_t* conn);
    static void doCleanup();  // 最后一次调用

    static bool _curlInit(conn_t* conn);
    static void _curlSetRequestHeader(conn_t* conn);
    static void _curlSetCommonOptions(conn_t* conn);
    static char* _doRequest(conn_t* conn);
    static size_t _curlOnResponseBodyRecv(void* ptr,
                                          size_t size,
                                          size_t nmemb,
                                          void* data);
    static size_t _curlOnResponseHeaderRecv(void* ptr,
                                            size_t size,
                                            size_t nmemb,
                                            void* data);
    static void _curlClose(conn_t* conn);
};

class Task {};

typedef struct _MultiRequest {
    CURL* handlers[8];  // 一次最多8个并发请求
    CURLM* multi_handler;
    std::thread thread_handler;
    std::vector<Task> tasks;

    int still_running = 1;
    int i;

    CURLMsg* msg;  /* for picking up messages with the transfer status */
    int msgs_left; /* how many messages are left */
} MutliRequest;