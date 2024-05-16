#include "net/ConcurrentRequest.h"

ConcurrentRequest::ConcurrentRequest(uint32_t concurrent_size)
    : m_concurrent_size(concurrent_size), m_successed(0), m_running(0), m_failed(0) {}

ConcurrentRequest::ConcurrentRequest(std::vector<conn_t>& connections,
                                     uint32_t concurrent_size)
    : m_concurrent_size(concurrent_size),
      m_connections(connections),
      m_successed(0),
      m_running(0),
      m_failed(0) {}

ConcurrentRequest::~ConcurrentRequest() {}

void ConcurrentRequest::AddConnection(conn_t& connection) {
    m_connections.push_back(connection);
}

void ConcurrentRequest::AddConnectionList(const std::vector<conn_t>& connections) {
    for (auto connection : connections) {
        m_connections.push_back(connection);
    }
}

// libCurl设置HTTPS响应回调函数
size_t ConcurrentRequest::_CurlOnResponseBodyRecv(void* ptr,
                                                  size_t size,
                                                  size_t nmemb,
                                                  void* data) {
    conn_t* conn = static_cast<conn_t*>(data);
    if (conn) {
        size_t recv_size = size * nmemb;
        conn->response.append(static_cast<char*>(ptr), recv_size);
    }
    return size * nmemb;
}

// libCurl设置HTTPS请求响应头回调函数
size_t ConcurrentRequest::_CurlOnResponseHeaderRecv(void* ptr,
                                                    size_t size,
                                                    size_t nmemb,
                                                    void* data) {
    size_t recv_size = size * nmemb;
    return recv_size;
}

// 初始化 LibCurl 依赖库
void ConcurrentRequest::_CurlInit(conn_t* conn) {
    conn->easy_curl = curl_easy_init();
    conn->response = "";
    if (conn->easy_curl) {
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
    delete conn;
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
        curl_easy_setopt(conn->easy_curl, CURLOPT_USERNAME,
                         conn->basic_auth_name.c_str());
    }

    if (conn->basic_auth_pwd.length() > 0) {
        curl_easy_setopt(conn->easy_curl, CURLOPT_PASSWORD, conn->basic_auth_pwd.c_str());
    }

    curl_easy_setopt(conn->easy_curl, CURLOPT_CONNECTTIMEOUT, conn->timeout);
    curl_easy_setopt(conn->easy_curl, CURLOPT_TIMEOUT, conn->timeout);
    curl_easy_setopt(conn->easy_curl, CURLOPT_WRITEFUNCTION,
                     ConcurrentRequest::_CurlOnResponseBodyRecv);
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
            curl_easy_setopt(conn->easy_curl, CURLOPT_INFILESIZE,
                             conn->upload_info->file_size);
            curl_easy_setopt(conn->easy_curl, CURLOPT_READDATA,
                             conn->upload_info->file_src);
            curl_easy_setopt(conn->easy_curl, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(conn->easy_curl, CURLOPT_USERAGENT, "chrome");
        } else {
            curl_easy_setopt(conn->easy_curl, CURLOPT_CUSTOMREQUEST, "POST");
            // 指定post数据大小，解决post数据为空时请求阻塞的问题
            if (conn->payload.length() == 0) {
                curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDSIZE, 0);
            } else {
                curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDSIZE,
                                 conn->payload.length());
                curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDS,
                                 conn->payload.c_str());
            }
        }
    } else if ((conn->method == "PUT") || (conn->method == "PATCH")) {
        curl_easy_setopt(conn->easy_curl, CURLOPT_CUSTOMREQUEST, conn->method);
        if (conn->payload.length() == 0) {
            curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDSIZE, 0);
        } else {
            curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDSIZE,
                             conn->payload.length());
            curl_easy_setopt(conn->easy_curl, CURLOPT_POSTFIELDS, conn->payload.c_str());
        }
    } else if (conn->method == "DELETE") {
        curl_easy_setopt(conn->easy_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
}

/**
 * @param cm CURLM*
 * @param i 发送的第几个请求
 */
void ConcurrentRequest::AddNewRequest(CURLM* cm, size_t i) {
    conn_t conn = m_connections[i];
    _CurlInit(&conn);
    curl_multi_add_handle(cm, conn.easy_curl);
    m_running += 1;
}

void ConcurrentRequest::Run() {
    CURLM* cm;
    CURLMsg* msg;
    int msgs_left = -1;
    size_t request_sended;  // 已经发送的请求下标

    curl_global_init(CURL_GLOBAL_ALL);
    cm = curl_multi_init();

    /* Limit the amount of simultaneous connections curl should allow: */
    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, static_cast<long>(m_concurrent_size));

    m_total = m_connections.size();

    for (request_sended = 0;
         request_sended < m_concurrent_size && request_sended < m_total;
         request_sended++) {
        AddNewRequest(cm, request_sended);
    }

    do {
        int still_alive = 1;
        curl_multi_perform(cm, &still_alive);

        while ((msg = curl_multi_info_read(cm, &msgs_left))) {
            if (msg->msg == CURLMSG_DONE) {
                conn_t* conn;
                CURL* easy_curl = msg->easy_handle;
                curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &conn);
                curl_easy_getinfo(conn->easy_curl, CURLINFO_RESPONSE_CODE,
                                  &conn->http_code);
                curl_easy_getinfo(conn->easy_curl, CURLINFO_TOTAL_TIME,
                                  &conn->total_time);
                if (conn->callback != nullptr && conn->http_code == 200) {
                    conn->callback(conn->response);
                }
                if (conn->debug) {
                    std::cout << "\n===============================\n";
                    std::cout << conn->response;
                    std::cout << "\n===============================" << std::endl;
                }
                curl_multi_remove_handle(cm, easy_curl);
                _CurlClose(conn);
                m_successed += 1;
                m_running--;
            } else {
                fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
                m_failed += 1;
            }
            if (request_sended < m_total) {
                AddNewRequest(cm, request_sended++);
            }
        }
        if (m_running) {
            curl_multi_wait(cm, NULL, 0, 1000, NULL);
        }

    } while (m_running);

    curl_multi_cleanup(cm);
    curl_global_cleanup();
}

uint32_t ConcurrentRequest::GetConcurrentSize() const {
    return m_concurrent_size;
}

size_t ConcurrentRequest::GetSuccessCount() {
    return m_successed;
}

size_t ConcurrentRequest::GetRunningCount() {
    return m_running;
}

size_t ConcurrentRequest::GetFailCount() {
    return m_failed;
}

size_t ConcurrentRequest::GetFinishCount() {
    return m_successed + m_failed;
}

void HttpConcurrentGet(const std::vector<std::string>& urls, uint32_t concurrent_size) {
    ConcurrentRequest request(concurrent_size);
    std::vector<conn_t> connections;
    for (auto url : urls) {
        conn_t conn;
        conn.url = url;
        conn.method = "GET";
        connections.push_back(conn);
    }
    request.AddConnectionList(connections);
    request.Run();
}

void HttpConcurrentGet(const std::vector<conn_t>& connections, uint32_t concurrent_size) {
    ConcurrentRequest request(concurrent_size);
    request.AddConnectionList(connections);
    request.Run();
}
