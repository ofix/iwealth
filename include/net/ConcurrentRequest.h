#pragma once
#include <iostream>
#include <list>
#include "../curl/curl.h"
#include "net/Conn.h"

class ConcurrentRequest {
   public:
    ConcurrentRequest(uint32_t concurrent_size = 3);
    ConcurrentRequest(std::list<conn_t*>& connections, uint32_t concurrent_size = 3);
    virtual ~ConcurrentRequest();
    void AddConnection(conn_t* connection);
    void AddConnectionList(const std::list<conn_t*>& connections);
    void Run();

   private:
    void AddNewRequest(CURLM* cm);
    static void _CurlInit(conn_t* conn);
    static void _SetRequestHeader(conn_t* conn);
    static void _SetCommonOptions(conn_t* conn);
    static void _SetMiscOptions(conn_t* conn);
    static void _CurlClose(conn_t* conn);
    static size_t _CurlOnResponseBodyRecv(void* ptr, size_t size, size_t nmemb, void* data);
    static size_t _CurlOnResponseHeaderRecv(void* ptr, size_t size, size_t nmemb, void* data);

   protected:
    uint32_t m_concurrent_size;        // 一次并发数
    std::list<conn_t*> m_connections;  // 所有请求
    size_t m_request_size;  // 用户需要发送的初始请求数，不包括请求衍生出来的子请求数量
};

void HttpConcurrentGet(const std::list<std::string>& urls,
                       std::function<void(conn_t*)>& callback,
                       void* user_extra,
                       uint32_t concurrent_size = 3);
void HttpConcurrentGet(const std::list<std::string>& urls,
                       std::function<void(conn_t*)>& callback,
                       const std::vector<void*>& user_extra,
                       uint32_t concurrent_size = 3);
void HttpConcurrentGet(const std::list<conn_t*>& connections, uint32_t concurrent_size = 3);