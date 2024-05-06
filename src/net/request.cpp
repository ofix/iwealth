#include "request.h"

Request* Request::m_pThis = NULL;

Request::Request() {
    curl_global_init(CURL_GLOBAL_ALL);
}

Request::~Request() {
    curl_global_cleanup();
}

Request* Request::Instance() {
    if (m_pThis == NULL) {
        m_pThis = new Request();
    }
    return m_pThis;
}

std::string Request::Get(std::string url) {
    conn_t conn;
    conn.url = url;
    conn.timeout = 50000;
    return Request::Get(&conn);
}

// 发送 HTTPS GET 请求
std::string Request::Get(conn_t* conn) {
    inner_conn_t* inner_conn = CurlInit(conn);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "GET");
    std::string response = DoRequest(conn, inner_conn);
    CurlClose(inner_conn);
    return response;
}

// 发送 HTTPS POST 请求
std::string Request::Post(conn_t* conn) {
    inner_conn_t* inner_conn = CurlInit(conn);
    if (conn->upload_info) {
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_TIMEOUT, 360L);
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_INFILESIZE,
                         conn->upload_info->file_size);
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_READDATA,
                         conn->upload_info->file_src);
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_USERAGENT, "chrome");
    } else {
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "POST");
        // 指定post数据大小，解决post数据为空时请求阻塞的问题
        if (conn->payload.length() == 0) {
            curl_easy_setopt(inner_conn->easy_curl, CURLOPT_POSTFIELDSIZE, 0);
        } else {
            curl_easy_setopt(inner_conn->easy_curl, CURLOPT_POSTFIELDSIZE,
                             conn->payload.length());
            curl_easy_setopt(inner_conn->easy_curl, CURLOPT_POSTFIELDS,
                             conn->payload.c_str());
        }
    }
    std::string response = DoRequest(conn, inner_conn);
    CurlClose(inner_conn);
    return response;
}

// 发送 HTTPS PUT 请求
std::string Request::Put(conn_t* conn) {
    inner_conn_t* inner_conn = CurlInit(conn);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "PUT");
    if (conn->payload.length() == 0) {
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_POSTFIELDSIZE, 0);
    } else {
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_POSTFIELDSIZE,
                         conn->payload.length());
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_POSTFIELDS,
                         conn->payload.c_str());
    }
    std::string response = DoRequest(conn, inner_conn);
    CurlClose(inner_conn);
    return response;
}

// 发送 HTTPS PATCH 请求
std::string Request::Patch(conn_t* conn) {
    inner_conn_t* inner_conn = CurlInit(conn);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    if (conn->payload.length() == 0) {
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_POSTFIELDSIZE, 0);
    } else {
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_POSTFIELDSIZE,
                         conn->payload.length());
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_POSTFIELDS,
                         conn->payload.c_str());
    }
    std::string response = DoRequest(conn, inner_conn);
    CurlClose(inner_conn);
    return response;
}

// 发送 HTTPS DELETE 请求
std::string Request::Delete(conn_t* conn) {
    inner_conn_t* inner_conn = CurlInit(conn);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    std::string response = DoRequest(conn, inner_conn);
    CurlClose(inner_conn);
    return response;
}

// 内部请求调用
std::string Request::DoRequest(conn_t* conn, inner_conn_t* inner_conn) {
    bool debug = conn->debug;  // curl_easy_perform 执行完将复位debug字段
    CURLcode res = curl_easy_perform(inner_conn->easy_curl);
    if (res != CURLE_OK) {
        if (conn->retry) {
            conn->retry--;
            return DoRequest(conn, inner_conn);
        }
    }
    // 设置HTTPS请求响应码
    curl_easy_getinfo(inner_conn->easy_curl, CURLINFO_RESPONSE_CODE, &conn->http_code);
    // 修正网络断开的时候，yyjson_read 解析长度为0的buf会崩溃的问题
    if (inner_conn->recv_size == 0 || conn->http_code == 0) {
        return NULL;
    }
    // conn->recv_buf_len 有缓存，多次请求后，是最大的那个请求的字节数；
    // 实际接收的字节数 <= 缓存空间 strlen(conn->recv_buf) <= conn->recv_buf_len
    // char* response = yyjson_read(inner_conn->recv_buf, strlen(inner_conn->recv_buf),
    // 0);
    if (debug) {
        std::cout << "\n===============================\n";
        std::cout << inner_conn->recv_buf;
        std::cout << "\n===============================" << std::endl;
    }
    return inner_conn->recv_buf;
}

// 初始化 LibCurl 依赖库
inner_conn_t* Request::CurlInit(conn_t* conn) {
    inner_conn_t* inner_conn = new inner_conn_t();
    inner_conn->easy_curl = curl_easy_init();
    inner_conn->recv_buf = "";
    inner_conn->recv_size = 0;
    if (inner_conn->easy_curl) {
        SetRequestHeader(conn, inner_conn);
        SetCommonOptions(conn, inner_conn);
    }
    return inner_conn;
}

// 设置HTTPS请求头，可选设置项
void Request::SetRequestHeader(conn_t* conn, inner_conn_t* inner_conn) {
    if (conn->headers.size() > 0) {
        struct curl_slist* list = NULL;
        for (int i = 0; conn->headers.size(); i++) {
            list = curl_slist_append(list, conn->headers[i].c_str());
        }
        list = curl_slist_append(list, "Expect: ");
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_HTTPHEADER, list);
        inner_conn->curl_header_list = list;
    }
}

// 设置curl通用设置项
void Request::SetCommonOptions(conn_t* conn, inner_conn_t* inner_conn) {
    // char url[BUF_SIZE] = {0};
    // strncpy(url, "https://", 9);  // https:/172.26.14.10
    // 也能发送成功，但不符合HTTPS标准 if (strstr(conn->hostname, ":")) {  // IPv6
    //     strncpy(url, "[", BUF_SIZE);
    //     strncpy(url, conn->hostname, BUF_SIZE);
    //     strncpy(url, "]", BUF_SIZE);
    // } else {
    //     strncpy(url, conn->hostname, BUF_SIZE);
    // }
    // strncpy(url, conn->url, BUF_SIZE);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_URL, conn->url.c_str());
    if (conn->basic_auth_name.length() > 0) {
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_USERNAME,
                         conn->basic_auth_name.c_str());
    }

    if (conn->basic_auth_pwd.length() > 0) {
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_PASSWORD,
                         conn->basic_auth_pwd.c_str());
    }
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_CONNECTTIMEOUT, conn->timeout);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_TIMEOUT, conn->timeout);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_WRITEFUNCTION,
                     Request::CurlOnResponseBodyRecv);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_WRITEDATA, inner_conn);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_HEADERFUNCTION,
                     Request::CurlOnResponseHeaderRecv);
    curl_easy_setopt(inner_conn->easy_curl, CURLOPT_HEADERDATA, inner_conn);
    if (conn->debug) {
        curl_easy_setopt(inner_conn->easy_curl, CURLOPT_VERBOSE, 1L);
    }
}

// libCurl设置HTTPS响应回调函数
size_t Request::CurlOnResponseBodyRecv(void* ptr, size_t size, size_t nmemb, void* data) {
    inner_conn_t* inner_conn = static_cast<inner_conn_t*>(data);
    if (inner_conn) {
        size_t recv_size = size * nmemb;
        inner_conn->recv_size += recv_size;
        inner_conn->recv_buf.append(static_cast<char*>(ptr), recv_size);
    }
    return size * nmemb;
}

// libCurl设置HTTPS请求响应头回调函数
size_t Request::CurlOnResponseHeaderRecv(void* ptr,
                                         size_t size,
                                         size_t nmemb,
                                         void* data) {
    size_t recv_size = size * nmemb;
    inner_conn_t* inner_conn = static_cast<inner_conn_t*>(data);
    if (inner_conn) {
        // char content_len[16];
        char* str_head = strstr(static_cast<char*>(ptr), "Content-Length: ");
        if (NULL == str_head) {
            return recv_size;
        }

        // char* str_end = strstr(str_head, "\r\n");
        // // 防止不规范的HTTP数据
        // if (NULL == str_end) {
        //     str_end = strstr(str_head, "\n");
        // }
        // size_t len = str_end - str_head - 16;
        // memcpy(content_len, str_end - len, len);
        // content_len[len] = '\0';
        // size_t buf_len = atoi(content_len);
        // if (inner_conn->recv_size < buf_len) {
        //     // 旧的空间不够，重新申请大的内存，否则复用即可
        //     if (inner_conn->recv_buf) {
        //         free(inner_conn->recv_buf);
        //         inner_conn->recv_buf = NULL;
        //     }
        //     inner_conn->recv_size = buf_len;
        // }
        // if (inner_conn->recv_buf) {

        // }
    }
    return recv_size;
}

// 清理curl资源
void Request::CurlClose(inner_conn_t* inner_conn) {
    curl_easy_cleanup(inner_conn->easy_curl);
    if (inner_conn->curl_header_list) {
        curl_slist_free_all(inner_conn->curl_header_list);
        inner_conn->curl_header_list = NULL;
    }
    if (inner_conn->recv_buf.length() > 0) {
        inner_conn->recv_buf.clear();
    }
    delete inner_conn;
}
