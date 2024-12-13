#pragma once

#include <functional>
#include <string>
#include <vector>
#include "curl.h"
#include "net/RequestStatistics.h"
#include "stock/Stock.h"

struct upload_info_t {
    FILE* file_src;
    long file_size;
    std::string file_name;
    std::string file_path;
};

struct CrawlExtra {
    RequestStatistics* statistics;  // 统计信息
};

class SpiderShareKlineProvider;
struct KlineCrawlExtra : CrawlExtra {
    SpiderShareKlineProvider* pProvider;
    KlineType type;  // K线类型，日/周/月/季度/年K线
    Market market;   // 深交所/北交所/上交所
    Share* share;    // StockDataStorage::m_market_shares 元素，
                     // 下载完数据不能释放此指针指向的对象
};

struct MinuteKlineCrawlExtra : CrawlExtra {
    SpiderShareKlineProvider* pProvider;
    KlineType type;  // K线类型，日/周/月/季度/年K线
    Market market;   // 深交所/北交所/上交所
    Share* share;    // StockDataStorage::m_market_shares 元素，
                     // 下载完数据不能释放此指针指向的对象
    std::vector<minuteKline>* ptr_minute_klines;
};

struct QuoteCrawlExtra : CrawlExtra {
    Market market;  // 深交所/北交所/上交所
};

struct BriefInfoCrawlExtra : CrawlExtra {
    Share* share;  //
};

struct CrawlRequest {
    std::string url;     // 请求URL
    CrawlExtra* pExtra;  // 用户额外数据
};

struct CategoryCrawlExtra : CrawlExtra {
    ShareCategoryType category_type;  // 板块类型(概念/行业/区域)
    std::string category_name;        // 板块名称
};

struct conn_t {
    std::string url;
    int http_version;
    std::string method;
    std::string payload;
    std::vector<std::string> headers;
    std::string status;
    upload_info_t* upload_info = nullptr;
    int http_code = 0;
    int retry = 1;       // 重试次数
    int timeout = 10000;  // 请求超时时间
    std::string basic_auth_name;
    std::string basic_auth_pwd;
    CURL* easy_curl = nullptr;                      // HTTP(s)请求句柄
    struct curl_slist* curl_header_list = nullptr;  // CURL 依赖头
    curl_off_t queue_time;                          // 请求排队时间
    curl_off_t total_time;                          // 请求响应总时间
    std::string response;                           // 返回响应数据
    bool reuse;             // 是否复用当前请求，比如百度财经的历史K线页面，
                            // 要分批次请求才能获取完整的历史K线数据
    void* extra = nullptr;  // 用户传入的额外数据,方便响应处理
    bool debug = false;
    std::function<void(conn_t*)> callback;  // HTTP(s)回调响应处理函数
    RequestStatistics* statistics;          // 统计指针
};