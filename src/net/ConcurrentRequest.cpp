///////////////////////////////////////////////////////////////////////////////
// Name:        src/net/ConcurrentRequest.cpp
// Purpose:     concurrent request based on libcurl
// Author:      songhuabiao
// Modified by:
// Created:     2024-05-14 12:35
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "net/ConcurrentRequest.h"
#include <iomanip>
#include "spider/Spider.h"
#include "util/EasyLogger.h"

ConcurrentRequest::ConcurrentRequest(const std::string& thread_name, int concurrent_size)
    : m_thread_name(thread_name), m_concurrent_size(concurrent_size), m_request_size(0), m_last_request_url("") {
}

ConcurrentRequest::ConcurrentRequest(const std::string& thread_name,
                                     std::list<conn_t*>& connections,
                                     int concurrent_size)
    : m_thread_name(thread_name),
      m_concurrent_size(concurrent_size),
      m_connections(connections),
      m_request_size(connections.size()),
      m_last_request_url("") {
}

ConcurrentRequest::~ConcurrentRequest() {
}

void ConcurrentRequest::AddConnection(conn_t* connection) {
    m_connections.push_back(connection);
    m_request_size += 1;
}

void ConcurrentRequest::AddConnectionList(const std::list<conn_t*>& connections) {
    for (auto connection : connections) {
        m_connections.push_back(connection);  // 插入到链表的尾部
        m_request_size += 1;
    }
}

// libCurl设置HTTPS响应回调函数
size_t ConcurrentRequest::_CurlOnResponseBodyRecv(void* ptr, size_t size, size_t nmemb, void* data) {
    conn_t* conn = static_cast<conn_t*>(data);
    size_t recv_size = size * nmemb;
    if (conn) {
        conn->response.append(static_cast<char*>(ptr), recv_size);
        // 这里需要增加数据统计
        conn->statistics->recv_bytes_cur += recv_size;
    }
    return recv_size;
}

// libCurl设置HTTPS请求响应头回调函数
size_t ConcurrentRequest::_CurlOnResponseHeaderRecv(void* /*ptr*/, size_t size, size_t nmemb, void* /*data*/) {
    size_t recv_size = size * nmemb;
    return recv_size;
}

// 初始化 LibCurl 依赖库
void ConcurrentRequest::_CurlInit(conn_t* conn, int http_version) {
    conn->easy_curl = curl_easy_init();
    conn->response = "";
    conn->reuse = false;  // 默认不复用请求
    if (conn->easy_curl) {
        curl_easy_setopt(conn->easy_curl, CURLOPT_HTTP_VERSION, http_version);
        _SetRequestHeader(conn);
        _SetCommonOptions(conn);
        _SetMiscOptions(conn);
    }
}

void ConcurrentRequest::_CurlClose(conn_t* conn) {
    curl_easy_cleanup(conn->easy_curl);
    if (conn->curl_header_list) {
        curl_slist_free_all(conn->curl_header_list);
        conn->curl_header_list = NULL;
    }
    if (conn->response.length() > 0) {
        conn->response.clear();
    }
    if (conn) {
        delete conn;
    }
}

void ConcurrentRequest::_CurlCloseRequestFailed(conn_t* conn) {
    curl_easy_cleanup(conn->easy_curl);
    if (conn->curl_header_list) {
        curl_slist_free_all(conn->curl_header_list);
        conn->curl_header_list = NULL;
    }
    if (conn->response.length() > 0) {
        conn->response.clear();
    }
}

void ConcurrentRequest::_SetRequestHeader(conn_t* conn) {
    if (conn->headers.size() > 0) {
        struct curl_slist* list = NULL;
        for (int i = 0; conn->headers.size(); i++) {
            list = curl_slist_append(list, conn->headers[i].c_str());
        }
        list = curl_slist_append(list, "Expect: ");
        curl_easy_setopt(conn->easy_curl, CURLOPT_HTTPHEADER, list);
        conn->curl_header_list = list;
    }
}

void ConcurrentRequest::_SetCommonOptions(conn_t* conn) {
    curl_easy_setopt(conn->easy_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(conn->easy_curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(conn->easy_curl, CURLOPT_URL, conn->url.c_str());
    curl_easy_setopt(conn->easy_curl, CURLOPT_PRIVATE, conn);
    if (conn->basic_auth_name.length() > 0) {
        curl_easy_setopt(conn->easy_curl, CURLOPT_USERNAME, conn->basic_auth_name.c_str());
    }

    if (conn->basic_auth_pwd.length() > 0) {
        curl_easy_setopt(conn->easy_curl, CURLOPT_PASSWORD, conn->basic_auth_pwd.c_str());
    }

    curl_easy_setopt(conn->easy_curl, CURLOPT_CONNECTTIMEOUT, conn->timeout);
    curl_easy_setopt(conn->easy_curl, CURLOPT_TIMEOUT, conn->timeout);
    curl_easy_setopt(conn->easy_curl, CURLOPT_WRITEFUNCTION, ConcurrentRequest::_CurlOnResponseBodyRecv);
    curl_easy_setopt(conn->easy_curl, CURLOPT_WRITEDATA, conn);
    // curl_easy_setopt(conn->easy_curl, CURLOPT_HEADERFUNCTION,
    //                  Request::CurlOnResponseHeaderRecv);
    // curl_easy_setopt(conn->easy_curl, CURLOPT_HEADERDATA, conn);
    if (conn->debug) {
        curl_easy_setopt(conn->easy_curl, CURLOPT_VERBOSE, 1L);
    }
}

void ConcurrentRequest::_SetMiscOptions(conn_t* conn) {
    if (conn->method == "GET") {
        curl_easy_setopt(conn->easy_curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(conn->easy_curl, CURLOPT_CUSTOMREQUEST, "GET");
    } else if (conn->method == "POST") {
        if (conn->upload_info) {
            curl_easy_setopt(conn->easy_curl, CURLOPT_CUSTOMREQUEST, "POST");
            curl_easy_setopt(conn->easy_curl, CURLOPT_TIMEOUT, conn->timeout);
            curl_easy_setopt(conn->easy_curl, CURLOPT_INFILESIZE, conn->upload_info->file_size);
            curl_easy_setopt(conn->easy_curl, CURLOPT_READDATA, conn->upload_info->file_src);
            curl_easy_setopt(conn->easy_curl, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(conn->easy_curl, CURLOPT_USERAGENT, "chrome");
        } else {
            curl_easy_setopt(conn->easy_curl, CURLOPT_CUSTOMREQUEST, "POST");
            // 指定post数据大小，解决post数据为空时请求阻塞的问题
            if (conn->payload.length() == 0) {
                curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDSIZE, 0);
            } else {
                curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDSIZE, conn->payload.length());
                curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDS, conn->payload.c_str());
            }
        }
    } else if ((conn->method == "PUT") || (conn->method == "PATCH")) {
        curl_easy_setopt(conn->easy_curl, CURLOPT_CUSTOMREQUEST, conn->method.c_str());
        if (conn->payload.length() == 0) {
            curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDSIZE, 0);
        } else {
            curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDSIZE, conn->payload.length());
            curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDS, conn->payload.c_str());
        }
    } else if (conn->method == "DELETE") {
        curl_easy_setopt(conn->easy_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
}

void ConcurrentRequest::AddNewRequest(CURLM* cm) {
    if (!m_connections.empty()) {
        conn_t* conn = m_connections.back();  // 获取最后一个元素
        bool old_reuse = conn->reuse;
        m_last_request_url = conn->url;
        _CurlInit(conn, conn->http_version);  // 每次初始化会复写reuse，导致百度财经无法正常结束
        conn->reuse = old_reuse;
        curl_multi_add_handle(cm, conn->easy_curl);
        m_connections.pop_back();  // 移除最后一个元素
        //////// 请求统计 /////////
        conn->statistics->ongoing_requests += 1;    // 新增一条请求
        conn->statistics->real_request_count += 1;  // 无论是否是复用的子请求，都统计在内
        //////////////////////////
    }
}

std::string ConcurrentRequest::GetThreadPrefix() const {
    return "[" + m_thread_name + "] ";
}

bool ConcurrentRequest::Run() {
    CURLM* cm;
    CURLMsg* msg;
    int msgs_left = -1;
    int request_no = 0;
    int request_success = 0;
    int request_fail = 0;
    int request_total = m_request_size;

    curl_global_init(CURL_GLOBAL_ALL);
    cm = curl_multi_init();

    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, static_cast<long>(m_concurrent_size));

    for (size_t i = 0; i < m_concurrent_size; i++) {
        AddNewRequest(cm);
    }

    for (;;) {
        int still_alive = 1;
        curl_multi_perform(cm, &still_alive);
        conn_t* conn;
        bool finished = false;
        while ((msg = curl_multi_info_read(cm, &msgs_left)) != NULL) {
            if (msg->msg == CURLMSG_DONE) {
                CURL* easy_curl = msg->easy_handle;
                curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &conn);
                curl_easy_getinfo(conn->easy_curl, CURLINFO_RESPONSE_CODE, &conn->http_code);
                curl_easy_getinfo(conn->easy_curl, CURLINFO_TOTAL_TIME, &conn->total_time);
                //////// 请求统计 /////////
                RequestStatistics* pStatistics = conn->statistics;
                if (pStatistics != nullptr) {
                    pStatistics->ongoing_requests -= 1;
                }

                bool response_parse_error = false;
                //////////////////////////
                if ((conn->callback) && conn->http_code == 200) {
                    try {
                        request_no += 1;
                        std::cout << std::setfill('0') << std::setw(3) << request_no << ": " << conn->url << std::endl;
                        conn->callback(conn);  // 回调函数中可能会设置 reuse 复用选项,如果reuse=true,不要释放conn
                    } catch (std::exception& e) {
                        response_parse_error = true;
                        std::cout << GetThreadPrefix() << "concurrent loop callback error! " << e.what() << std::endl;
                    }
                }
                curl_multi_remove_handle(cm, easy_curl);
                if (!conn->reuse) {  // 如果没有子请求复用，释放资源
                    //////// 请求统计 /////////
                    if (conn->http_code == 200) {
                        if (pStatistics != nullptr) {
                            pStatistics->success_requests += 1;
                        }
                        request_success += 1;
                        std::cout << GetThreadPrefix() << "success: " << pStatistics->success_requests
                                  << ", failed: " << pStatistics->failed_requests
                                  << ", total: " << pStatistics->request_count << std::endl;
                        _CurlClose(conn);  // 修正windows下提前关闭conn，导致上面统计报错的问题
                    } else {
                        if (pStatistics != nullptr) {
                            pStatistics->failed_requests += 1;
                        }
                        request_fail += 1;
                        if (curl_easy_getinfo(conn->easy_curl, CURLINFO_EFFECTIVE_URL, &conn->url) == CURLE_OK) {
                            std::cout << std::setfill('0') << std::setw(3) << request_no << ": " << conn->url
                                      << ", http_code = " << conn->http_code << std::endl;
                        }
                        _CurlCloseRequestFailed(conn);
                    }
                    //////////////////////////
                } else {  // 如果复用，需要重置response
                    conn->response.clear();
                    AddConnection(conn);  // 再次发送子请求
                }
                AddNewRequest(cm);
                finished = request_success + request_fail >= request_total;
            } else {
                std::cerr << GetThreadPrefix() << "E:CURLMsg " << msg->msg << std::endl;
            }
        }
        // 检查是否有进行中的请求
        if (still_alive || !finished) {
            curl_multi_wait(cm, NULL, 0, 1000, NULL);
        } else {
            if (!conn->reuse) {
                break;
            }
        }
    }

    curl_multi_cleanup(cm);
    curl_global_cleanup();
    return request_fail > 0 ? false : true;
}

bool HttpConcurrentGet(const std::vector<CrawlRequest>& requests,
                       std::function<void(conn_t*)>& callback,
                       int concurrent_size,
                       std::string thread_name,
                       RequestStatistics* pStatistics,
                       int http_version) {
    ConcurrentRequest request(thread_name, concurrent_size);
    std::list<conn_t*> connections;
    for (auto request : requests) {
        conn_t* pConn = new conn_t();  // 必须堆上分配，否则_CurlClose删除conn会报错！
        if (!pConn) {
            gLogger->log("%s line %d error", __FILE__, __LINE__);
            return false;
        }
        pConn->url = request.url;
        pConn->http_version = http_version;
        pConn->method = "GET";
        pConn->callback = callback;
        pConn->extra = request.pExtra;
        pConn->debug = false;
        pConn->statistics = pStatistics;
        connections.push_back(pConn);
    }
    request.AddConnectionList(connections);
    return request.Run();
}
