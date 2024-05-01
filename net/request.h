#pragma once
#include "conn.h"

#define BUF_SIZE 1024
#define HTTPS_TIME_OUT_MIN 15
#define HTTPS_TIME_OUT_MAX 20

char *doGet(conn_t *conn);
char *doPost(conn_t *conn);
char *doPut(conn_t *conn);
char *doPatch(conn_t *conn);
char *doDelete(conn_t *conn);
void doCleanup(); // 最后一次调用

bool _curlInit(conn_t *conn);
void _curlSetRequestHeader(conn_t *conn);
void _curlSetCommonOptions(conn_t *conn);
char *_doRequest(conn_t *conn);
static size_t _curlOnResponseBodyRecv(void *ptr, size_t size, size_t nmemb, void *data);
static size_t _curlOnResponseHeaderRecv(void *ptr, size_t size, size_t nmemb, void *data);
void _curlClose(conn_t *conn);
