﻿#pragma once

#include <stdint.h>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <unordered_map>

#define TIMER_WHEEL_SLOT_BITS 6
#define TIMER_WHEEL_SLOTS 1 << TIMER_WHEEL_SLOT_BITS
#define TIMER_WHEELS 5  // 5个时间轮
#define TIMER_MASK ((1 << TIMER_WHEEL_SLOT_BITS) - 1)
#define TIMER_TICK_MS 50  // 最小时间间隔50ms
#define TIMER_CREATE_FAILED 0

class TimerTask {
   public:
    TimerTask* next;    // 下一个定时任务
    uint32_t timer_id;  // 定时ID，用于取消定时任务
    uint32_t expire;    // 定时任务第一次执行间隔，单位ms
    uint32_t interval;  // 定时任务间隔
    bool is_period;     // 是否是周期性定时任务
    bool canceled;      // 定时任务是否取消
    void (*callback)(uint32_t timer_id, void* args);
    void* args;
};

/**
 * @todo 实现机制
 * 1. 用户调用 SetTimeout 或者 SetInterval 静态函数，会实例化Timer单例，并启动分离线程
 * 2. 用户再次调用的时候就无需启动线程，因为Timer类所在线程和定时器工作线程是独立的，
 *    因此操作Timer对象的数据成员需要加锁
 * @example
 *
 * ```
 * // 添加一次性定时器
 * Timer::SetTimeout(500,[&](uint32_t timer_id, void* args){
 *     std::cout<< "Timer[<<"<<timer_id<<"], timeout: 500ms"<<std::endl;
 * });
 *
 * // 添加周期性定时器
 * int loop = 0;
 * Timer::SetInterval(500,[&](uint32_t timer_id, void* args){
 *     std::cout<< "Timer[<<"<<timer_id<<"], interval: 500ms"<<std::endl;
 *     loop += 1;
 *     if(loop==5){
 *          // 取消周期性定时任务
 *          Timer::CancelTimer(timer_id);
 *     }
 * });
 * ```
 */
class Timer {
   public:
    Timer();
    ~Timer();
    static Timer* GetInstance();
    // 禁止复制，赋值，移动，移动赋值构造
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

   public:  // 对外调用函数
    static uint32_t SetTimeout(int timeout,
                               void (*callback)(uint32_t timer_id, void* args),
                               void* arguments = NULL);
    static uint32_t SetInterval(int interval,
                                void (*callback)(uint32_t timer_id, void* args),
                                int delay_time = 0,
                                void* arguments = NULL);
    static bool CancelTimer(uint32_t timer_id);
    static void CancelAllTimer();

   private:
    std::unordered_map<uint32_t, TimerTask*> GetTimerMap();
    void IncrementTimerId();
    uint32_t GetTimerId();
    int64_t CurrentTime();
    bool CancelTimerInternal(uint32_t timer_id);
    void CancelAllTimerInternal();
    uint32_t AddTimerTask(bool is_period,
                          int interval,
                          void (*callback)(uint32_t timer_id, void* args),
                          int delay_time = 0,
                          void* arguments = NULL);
    void AddTimerTask(TimerTask* timer_task, uint8_t wheel_index, uint8_t slot_index);
    void DelTimerTask(TimerTask* timer_task);
    int64_t CurrentTimeInMilliSecond();
    void CascadeTimer(uint32_t wi, uint32_t si);
    void Schedule();  // 使用额外的线程进行定时
    void TimerLoop();
    static Timer* instance;
    int64_t m_current_time;  // 时间精度 100 ms
    uint32_t m_timer_id;     // 当前定时类，永远递增
    std::mutex m_mutex;      // 互斥锁
    std::thread m_timer_thread;
    std::unordered_map<uint32_t, TimerTask*>
        m_timer_map;  // 定时器map，方便快速删除定时器
    TimerTask* head[TIMER_WHEELS][TIMER_WHEEL_SLOTS];
    TimerTask* tail[TIMER_WHEELS][TIMER_WHEEL_SLOTS];
};