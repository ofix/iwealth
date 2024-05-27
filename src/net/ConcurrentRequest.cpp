#include "net/ConcurrentRequest.h"
#include "util/EasyLogger.h"

size_t ConcurrentRequest::m_recv_total_bytes = 0;

ConcurrentRequest::ConcurrentRequest(uint32_t concurrent_size)
    : m_request_size(0), m_concurrent_size(concurrent_size), m_successed(0), m_running(0), m_failed(0) {}

ConcurrentRequest::ConcurrentRequest(std::list<conn_t*>& connections, uint32_t concurrent_size)
    : m_concurrent_size(concurrent_size),
      m_request_size(0),
      m_connections(connections),
      m_successed(0),
      m_running(0),
      m_failed(0) {}

ConcurrentRequest::~ConcurrentRequest() {}

void ConcurrentRequest::AddConnection(conn_t* connection) {
    m_connections.push_back(connection);
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
        m_recv_total_bytes += recv_size;
        // m_time_current = now();
        conn->response.append(static_cast<char*>(ptr), recv_size);
    }
    return recv_size;
}

// libCurl设置HTTPS请求响应头回调函数
size_t ConcurrentRequest::_CurlOnResponseHeaderRecv(void* ptr, size_t size, size_t nmemb, void* data) {
    size_t recv_size = size * nmemb;
    return recv_size;
}

// 初始化 LibCurl 依赖库
void ConcurrentRequest::_CurlInit(conn_t* conn) {
    conn->easy_curl = curl_easy_init();
    conn->response = "";
    conn->reuse = false;  // 默认不复用请求
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
        curl_easy_setopt(conn->easy_curl, CURLOPT_CUSTOMREQUEST, conn->method);
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

/**
 * @param cm CURLM*
 * @param i 发送的第几个请求
 */
void ConcurrentRequest::AddNewRequest(CURLM* cm) {
    if (!m_connections.empty()) {
        conn_t* conn = m_connections.back();  // 获取最后一个元素
        _CurlInit(conn);
        curl_multi_add_handle(cm, conn->easy_curl);
        m_running += 1;
        m_connections.pop_back();  // 移除最后一个元素
    }
}

void ConcurrentRequest::Run() {
    CURLM* cm;
    CURLMsg* msg;
    int msgs_left = -1;

    curl_global_init(CURL_GLOBAL_ALL);
    cm = curl_multi_init();

    /* Limit the amount of simultaneous connections curl should allow: */
    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, static_cast<long>(m_concurrent_size));

    m_total = m_request_size;

    for (size_t i = 0; i < m_concurrent_size; i++) {
        AddNewRequest(cm);
    }

    do {
        int still_alive = 1;
        curl_multi_perform(cm, &still_alive);

        while ((msg = curl_multi_info_read(cm, &msgs_left))) {
            if (msg->msg == CURLMSG_DONE) {
                conn_t* conn;
                CURL* easy_curl = msg->easy_handle;
                curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &conn);
                curl_easy_getinfo(conn->easy_curl, CURLINFO_RESPONSE_CODE, &conn->http_code);
                curl_easy_getinfo(conn->easy_curl, CURLINFO_TOTAL_TIME, &conn->total_time);
                if ((conn->callback) && conn->http_code == 200) {
                    conn->callback(conn);
                }
                curl_multi_remove_handle(cm, easy_curl);
                _CurlClose(conn);
                m_successed += 1;
                m_running--;
            } else {
                fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
                m_failed += 1;
            }
            AddNewRequest(cm);
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

// 获取平均响应速度
double ConcurrentRequest::GetAverageSpeed() {
    double speed = static_cast<double>(m_recv_total_bytes) / (m_time_current - m_time_start);
    return speed;
}

void HttpConcurrentGet(const std::list<std::string>& urls,
                       std::function<void(conn_t*)>& callback,
                       void* user_extra,
                       uint32_t concurrent_size) {
    ConcurrentRequest request(concurrent_size);
    std::list<conn_t*> connections;
    for (auto url : urls) {
        conn_t* pConn = new conn_t();
        if (!pConn) {
            gLogger->log("%s line %d error", __FILE__, __LINE__);
            break;
        }
        pConn->url = url;
        pConn->method = "GET";
        pConn->callback = callback;
        pConn->extra = user_extra;
        connections.push_back(pConn);
    }
    request.AddConnectionList(connections);
    request.Run();
}

void HttpConcurrentGet(const std::list<std::string>& urls,
                       std::function<void(conn_t*)>& callback,
                       const std::vector<void*>& user_extra,
                       uint32_t concurrent_size) {
    ConcurrentRequest request(concurrent_size);
    std::list<conn_t*> connections;
    size_t i = 0;
    for (auto url : urls) {
        conn_t* pConn = new conn_t();
        if (!pConn) {
            gLogger->log("%s line %d error", __FILE__, __LINE__);
            break;
        }
        pConn->url = url;
        pConn->method = "GET";
        pConn->callback = callback;
        pConn->extra = user_extra[i++];
        connections.push_back(pConn);
    }
    request.AddConnectionList(connections);
    request.Run();
}

void HttpConcurrentGet(const std::list<conn_t*>& connections, uint32_t concurrent_size) {
    ConcurrentRequest request(concurrent_size);
    request.AddConnectionList(connections);
    request.Run();
}
