#pragma once

#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include "../curl/curl.h"
#include "net/Conn.h"

#define BUF_SIZE 1024
#define HTTPS_TIME_OUT_MIN 15
#define HTTPS_TIME_OUT_MAX 20
#define gRequest Request::Instance()

class Request {
   public:
    static Request* Instance();
    virtual ~Request();
    static std::string Get(const std::string& url,
                           int http_version = CURL_HTTP_VERSION_1_1);
    static std::string Get(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
    static std::string Post(const std::string& url,
                            const std::string& payload,
                            int http_version = CURL_HTTP_VERSION_1_1);
    static std::string Post(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
    static std::string Put(const std::string& url,
                           const std::string& payload,
                           int http_version = CURL_HTTP_VERSION_1_1);
    static std::string Put(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
    static std::string Patch(const std::string& url,
                             const std::string& payload,
                             int http_version = CURL_HTTP_VERSION_1_1);
    static std::string Patch(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
    static std::string Delete(const std::string& url,
                              int http_version = CURL_HTTP_VERSION_1_1);
    static std::string Delete(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);

   private:
    static void CurlInit(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
    static void SetRequestHeader(conn_t* conn);
    static void SetCommonOptions(conn_t* conn);
    static std::string DoRequest(conn_t* conn);
    static size_t CurlOnResponseBodyRecv(void* ptr,
                                         size_t size,
                                         size_t nmemb,
                                         void* data);
    static size_t CurlOnResponseHeaderRecv(void* ptr,
                                           size_t size,
                                           size_t nmemb,
                                           void* data);
    static void CurlClose(conn_t* conn);

    Request();
    Request(const Request&){};
    Request& operator=(const Request&) { return *this; };
    static Request* m_pThis;
};

std::string HttpGet(const std::string& url, int http_version = CURL_HTTP_VERSION_1_1);
std::string HttpGet(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
std::string HttpPost(const std::string& url,
                     const std::string& payload,
                     int http_version = CURL_HTTP_VERSION_1_1);
std::string HttpPost(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
std::string HttpPut(const std::string& url,
                    const std::string& payload,
                    int http_version = CURL_HTTP_VERSION_1_1);
std::string HttpPut(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
std::string HttpPatch(const std::string& url,
                      const std::string& payload,
                      int http_version = CURL_HTTP_VERSION_1_1);
std::string HttpPatch(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);
std::string HttpDelete(const std::string& url, int http_version = CURL_HTTP_VERSION_1_1);
std::string HttpDelete(conn_t* conn, int http_version = CURL_HTTP_VERSION_1_1);