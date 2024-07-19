#pragma once
#include <curl.h>
#include <iostream>
#include <list>
#include <string>
#include "net/Conn.h"

class ConcurrentRequest {
   public:
    ConcurrentRequest(const std::string& thread_name, int concurrent_size = 3);
    ConcurrentRequest(const std::string& thread_name, std::list<conn_t*>& connections, int concurrent_size = 3);
    virtual ~ConcurrentRequest();
    void AddConnection(conn_t* connection);
    void AddConnectionList(const std::list<conn_t*>& connections);
    bool Run();
    std::string GetThreadPrefix() const;

   private:
    void AddNewRequest(CURLM* cm);
    static void _CurlInit(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
    static void _SetRequestHeader(conn_t* conn);
    static void _SetCommonOptions(conn_t* conn);
    static void _SetMiscOptions(conn_t* conn);
    static void _CurlClose(conn_t* conn);
    static void _CurlCloseRequestFailed(conn_t* conn);
    static size_t _CurlOnResponseBodyRecv(void* ptr, size_t size, size_t nmemb, void* data);
    static size_t _CurlOnResponseHeaderRecv(void* ptr, size_t size, size_t nmemb, void* data);

   protected:
    std::string m_thread_name;         // 线程名称
    size_t m_concurrent_size;          // 一次并发数
    std::list<conn_t*> m_connections;  // 所有请求
    size_t m_request_size;  // 用户需要发送的初始请求数，不包括请求衍生出来的子请求数量
    std::string m_last_request_url;  // 上一次请求的URL完整地址，防止接口403报错，无法从conn->url中获取到
};

bool HttpConcurrentGet(const std::vector<CrawlRequest>& requests,
                       std::function<void(conn_t*)>& callback,
                       int concurrent_size = 3,
                       std::string thread_name = "",
                       RequestStatistics* pStatistics = nullptr,
                       int http_version = CURL_HTTP_VERSION_1_1);