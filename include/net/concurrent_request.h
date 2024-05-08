#pragma once
#include <iostream>
#include <vector>
#include "../curl/curl.h"
#include "conn.h"

class ConcurrentRequest {
   public:
    ConcurrentRequest(uint32_t concurrent_size = 3);
    ConcurrentRequest(std::vector<conn_t>& connections, uint32_t concurrent_size = 3);
    virtual ~ConcurrentRequest();
    void AddConnection(conn_t& connection);
    void AddConnectionList(std::vector<conn_t>& connections);
    void Run();
    uint32_t GetConcurrentSize() const;
    size_t GetSuccessCount();
    size_t GetRunningCount();
    size_t GetFailCount();
    size_t GetFinishCount();
    void AddNewRequest(CURLM* cm, size_t i);

   protected:
    static inner_conn_t* _CurlInit(conn_t* conn);
    static void _SetRequestHeader(conn_t* conn, inner_conn_t* inner_conn);
    static void _SetCommonOptions(conn_t* conn, inner_conn_t* inner_conn);
    static void _SetMiscOptions(conn_t* conn, inner_conn_t* inner_conn);
    static size_t _CurlOnResponseBodyRecv(void* ptr,
                                          size_t size,
                                          size_t nmemb,
                                          void* data);
    static size_t _CurlOnResponseHeaderRecv(void* ptr,
                                            size_t size,
                                            size_t nmemb,
                                            void* data);

   protected:
    uint32_t m_concurrent_size;         // 一次并发数
    std::vector<conn_t> m_connections;  // 所有请求
    size_t m_successed;                 // 成功请求数
    size_t m_running;                   // 进行中请求数
    size_t m_failed;                    // 失败请求数
    size_t m_total;                     // 所有请求数
};