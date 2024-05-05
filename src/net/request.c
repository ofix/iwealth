#include "request.h"
#include <string.h>

static bool _curl_global_inited_ = false;

// 发送 HTTPS GET 请求
char* Request::doGet(conn_t* conn) {
    _curlInit(conn);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "GET");
    char* response = _doRequest(conn);
    _curlClose(conn);
    return response;
}

// 发送 HTTPS POST 请求
char* Request::doPost(conn_t* conn) {
    _curlInit(conn);
    if (conn->upload_info && conn->upload_info->upload) {
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_TIMEOUT, 360L);
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_INFILESIZE,
                         conn->upload_info->file_size);
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_READDATA,
                         conn->upload_info->file_src);
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_USERAGENT, "chrome");
    } else {
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "POST");
        // 指定post数据大小，解决post数据为空时请求阻塞的问题
        if (NULL == conn->post_data)
            curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_POSTFIELDSIZE, 0);
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_POSTFIELDS, conn->post_data);
    }
    char* response = _doRequest(conn);
    _curlClose(conn);
    return response;
}

// 发送 HTTPS PUT 请求
char* Request::doPut(conn_t* conn) {
    _curlInit(conn);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "PUT");
    if (NULL == conn->post_data)
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_POSTFIELDSIZE, 0);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_POSTFIELDS, conn->post_data);
    char* response = _doRequest(conn);
    _curlClose(conn);
    return response;
}

// 发送 HTTPS PATCH 请求
char* Request::doPatch(conn_t* conn) {
    _curlInit(conn);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    if (NULL == conn->post_data)
        curl_easy_setopt(conn->easy_conn, CURLOPT_POSTFIELDSIZE, 0);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_POSTFIELDS, conn->post_data);
    char* response = _doRequest(conn);
    _curlClose(conn);
    return response;
}

// 发送 HTTPS DELETE 请求
char* Request::doDelete(conn_t* conn) {
    _curlInit(conn);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    char* response = _doRequest(conn);
    _curlClose(conn);
    return response;
}

// 内部请求调用
char* Request::_doRequest(conn_t* conn) {
    bool debug = conn->debug;  // curl_easy_perform 执行完将复位debug字段
    CURLcode res = curl_easy_perform(conn->easy_conn->easy_curl);
    if (res != CURLE_OK) {
        if (conn->again) {
            conn->again--;
            return _doRequest(conn);
        }
    }
    // 设置HTTPS请求响应码
    curl_easy_getinfo(conn->easy_conn->easy_curl, CURLINFO_RESPONSE_CODE,
                      &conn->http_code);
    // 修正网络断开的时候，yyjson_read 解析长度为0的buf会崩溃的问题
    if (conn->recive_buff_len == 0 || conn->http_code == 0) {
        return NULL;
    }
    // conn->recive_buff_len 有缓存，多次请求后，是最大的那个请求的字节数；
    // 实际接收的字节数 <= 缓存空间 strlen(conn->recive_buff) <= conn->recive_buff_len
    char* response = yyjson_read(conn->recive_buff, strlen(conn->recive_buff), 0);
    if (debug) {
        printf(
            "\n===============================\n%s\n============================"
            "===\n",
            conn->recive_buff);
    }
    return response;
}

// 初始化 LibCurl 依赖库
bool Request::_curlInit(conn_t* conn) {
    if (!_curl_global_inited_) {
        curl_global_init(CURL_GLOBAL_ALL);
        _curl_global_inited_ = true;
    }
    conn->easy_conn = malloc(sizeof(ConnInfo));
    conn->easy_conn->easy_curl = curl_easy_init();
    if (conn->easy_conn->easy_curl) {
        _curlSetRequestHeader(conn);
        _curlSetCommonOptions(conn);
    }
    return true;
}

// 设置HTTPS请求头，可选设置项
void Request::_curlSetRequestHeader(conn_t* conn) {
    if (conn->headers[0]) {
        struct curl_slist* list = NULL;
        for (int i = 0; i < 3 && conn->headers[i]; i++) {
            list = curl_slist_append(list, conn->headers[i]);
        }
        list = curl_slist_append(list, "Expect: ");
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_HTTPHEADER, list);
        conn->easy_conn->list = list;
    }
}

// 设置curl通用设置项
void Request::_curlSetCommonOptions(conn_t* conn) {
    char url[BUF_SIZE] = {0};
    g_strlcpy(url, "https://", 9);  // https:/172.26.14.10 也能发送成功，但不符合HTTPS标准
    if (strstr(conn->hostname, ":")) {  // IPv6
        g_strlcat(url, "[", BUF_SIZE);
        g_strlcat(url, conn->hostname, BUF_SIZE);
        g_strlcat(url, "]", BUF_SIZE);
    } else {
        g_strlcat(url, conn->hostname, BUF_SIZE);
    }
    g_strlcat(url, conn->url_path, BUF_SIZE);
    // 如果请求出错，请打开调试模式
    // conn->debug = true;
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_URL, url);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_USERNAME, conn->username);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_PASSWORD, conn->password);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_CONNECTTIMEOUT, conn->timeout);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_TIMEOUT, conn->timeout);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_WRITEFUNCTION,
                     _curlOnResponseBodyRecv);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_WRITEDATA, conn);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_HEADERFUNCTION,
                     _curlOnResponseHeaderRecv);
    curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_HEADERDATA, conn);
    if (conn->debug) {
        curl_easy_setopt(conn->easy_conn->easy_curl, CURLOPT_VERBOSE, 1L);
    }
}

// libCurl设置HTTPS响应回调函数
size_t Request::_curlOnResponseBodyRecv(void* ptr,
                                        size_t size,
                                        size_t nmemb,
                                        void* data) {
    curl_conn_t* conn = (curl_conn_t*)data;
    if (conn) {
        if (NULL == conn->recive_buff) {
            conn->recive_buff = malloc(conn->recive_buff_len + 1);
            memset(conn->recive_buff, 0, conn->recive_buff_len + 1);
        }
        strcat(conn->recive_buff, (char*)ptr);
    }
    return size * nmemb;
}

// libCurl设置HTTPS请求响应头回调函数
size_t Request::_curlOnResponseHeaderRecv(void* ptr,
                                          size_t size,
                                          size_t nmemb,
                                          void* data) {
    size_t recv_size = size * nmemb;
    curl_conn_t* conn = (curl_conn_t*)data;
    if (conn) {
        char content_len[16];
        char* str_head = strstr(ptr, "Content-Length: ");
        if (NULL == str_head)
            return recv_size;

        char* str_end = strstr(str_head, "\r\n");
        // 防止不规范的HTTP数据
        if (NULL == str_end)
            str_end = strstr(str_head, "\n");

        size_t len = str_end - str_head - 16;
        memcpy(content_len, str_end - len, len);
        content_len[len] = '\0';
        size_t buff_len = atoi(content_len);
        if (conn->recive_buff_len < buff_len) {
            // 旧的空间不够，重新申请大的内存，否则复用即可
            if (conn->recive_buff) {
                free(conn->recive_buff);
                conn->recive_buff = NULL;
            }
            conn->recive_buff_len = buff_len;
        }
        if (conn->recive_buff)
            conn->recive_buff[0] = '\0';
    }
    return recv_size;
}

// 清理curl资源
void Request::_curlClose(conn_t* conn) {
    curl_easy_cleanup(conn->easy_conn->easy_curl);
    if (conn->easy_conn->list) {
        curl_slist_free_all(conn->easy_conn->list);
        conn->easy_conn->list = NULL;
    }
    if (conn->recive_buff) {
        free(conn->recive_buff);
        conn->recive_buff = NULL;
    }
    free(conn->easy_conn);
}

// 所有请求完成后调用一次
void Request::doCleanup() {
    if (_curl_global_inited_) {
        curl_global_cleanup();
        _curl_global_inited_ = false;
    }
}
