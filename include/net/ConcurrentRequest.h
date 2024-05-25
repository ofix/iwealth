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
    uint32_t GetConcurrentSize() const;
    size_t GetSuccessCount();
    size_t GetRunningCount();
    size_t GetFailCount();
    size_t GetFinishCount();
    double GetAverageSpeed();  // 获取响应速度

   private:
    void AddNewRequest(CURLM* cm);
    static void _CurlInit(conn_t* conn);
    static void _SetRequestHeader(conn_t* conn);
    static void _SetCommonOptions(conn_t* conn);
    static void _SetMiscOptions(conn_t* conn);
    static void _CurlClose(conn_t* conn);
    static size_t _CurlOnResponseBodyRecv(void* ptr,
                                          size_t size,
                                          size_t nmemb,
                                          void* data);
    static size_t _CurlOnResponseHeaderRecv(void* ptr,
                                            size_t size,
                                            size_t nmemb,
                                            void* data);

   protected:
    uint32_t m_concurrent_size;        // 一次并发数
    std::list<conn_t*> m_connections;  // 所有请求
    size_t m_request_size;  // 用户需要发送的初始请求数，不包括请求衍生出来的子请求数量
    size_t m_successed;                // 成功请求数
    size_t m_running;                  // 进行中请求数
    size_t m_failed;                   // 失败请求数
    size_t m_total;                    // 所有请求数
    time_t m_time_start;               // 请求开始时间
    time_t m_time_current;             // 当前时间
    static size_t m_recv_total_bytes;  // 所有请求已接收字节数
    long m_request_total_time;         // 所有请求平均响应时间
    size_t m_request_avg_speed;        // 请求平均速度
};

void HttpConcurrentGet(const std::list<std::string>& urls,
                       std::function<void(conn_t*)>& callback,
                       void* user_extra,
                       uint32_t concurrent_size = 3);
void HttpConcurrentGet(const std::list<conn_t*>& connections,
                       uint32_t concurrent_size = 3);