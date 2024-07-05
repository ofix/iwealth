#pragma once
#include <iostream>
#include <list>
#include <string>
#include "../curl/curl.h"
#include "net/Conn.h"

class ConcurrentRequest {
   public:
    ConcurrentRequest(const std::string& thread_name, int concurrent_size = 3);
    ConcurrentRequest(const std::string& thread_name, std::list<conn_t*>& connections, int concurrent_size = 3);
    virtual ~ConcurrentRequest();
    void AddConnection(conn_t* connection);
    void AddConnectionList(const std::list<conn_t*>& connections);
    void Run();
    std::string GetThreadPrefix() const;

   private:
    void AddNewRequest(CURLM* cm);
    static void _CurlInit(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
    static void _SetRequestHeader(conn_t* conn);
    static void _SetCommonOptions(conn_t* conn);
    static void _SetMiscOptions(conn_t* conn);
    static void _CurlClose(conn_t* conn);
    static size_t _CurlOnResponseBodyRecv(void* ptr, size_t size, size_t nmemb, void* data);
    static size_t _CurlOnResponseHeaderRecv(void* ptr, size_t size, size_t nmemb, void* data);

   protected:
    std::string m_thread_name;         // 线程名称
    int m_concurrent_size;             // 一次并发数
    std::list<conn_t*> m_connections;  // 所有请求
    size_t m_request_size;  // 用户需要发送的初始请求数，不包括请求衍生出来的子请求数量
    std::string m_last_request_url;  // 上一次请求的URL完整地址，防止接口403报错，无法从conn->url中获取到
};

void HttpConcurrentGet(const std::string& thread_name,
                       const std::list<std::string>& urls,
                       std::function<void(conn_t*)>& callback,
                       void* user_extra,
                       int concurrent_size = 3,
                       int http_version = CURL_HTTP_VERSION_1_1);
void HttpConcurrentGet(const std::string& thread_name,
                       const std::list<std::string>& urls,
                       std::function<void(conn_t*)>& callback,
                       const std::vector<void*>& user_extra,
                       int concurrent_size = 3,
                       int http_version = CURL_HTTP_VERSION_1_1);
void HttpConcurrentGet(const std::string& thread_name, const std::list<conn_t*>& connections, int concurrent_size = 3);