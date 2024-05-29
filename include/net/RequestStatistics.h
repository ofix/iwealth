#pragma once
#include <atomic>
#include <mutex>
#include <vector>
#include "stock/Stock.h"

struct RequestStatistics {
    KlineProvider provider;  // K线爬取网站，东方财富|百度财经|腾讯财经|新浪财经|和讯网，只需初始化一次
    std::atomic<uint32_t> request_count;  // 请求总数，只需初始化一次
    ////
    std::atomic<size_t> recv_bytes_last;  // 上次采样间隔接收的字节总数,存在多个线程同时读写
    std::atomic<size_t> recv_bytes_cur;   // 本次采样间隔接收的字节总数，存在多个线程同时读写
    ////
    std::atomic<uint32_t> real_request_count;  // 实际发送的请求数，有些网站需要发送多次请求才能获取完整的历史K线
    std::atomic<uint32_t> success_requests;  // 发送成功的请求数(不计算子请求)
    std::atomic<uint32_t> failed_requests;   // 发送失败的请求数(不计算子请求)
    std::atomic<uint32_t> ongoing_requests;  // 正在进行中的请求数(不计算子请求)
    ////
    std::atomic<size_t> real_time_speed;  // 实时采样速度，单位: 字节/秒，定时线程每隔1秒更新
    std::vector<size_t> speed_list;       // 历史速度，定时线程每隔1秒压入一条记录
    std::atomic<double> progress;         // 爬取进度
    std::mutex mutex;                     // 历史速度锁
};