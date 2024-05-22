#include "util/Timer.h"
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <cassert>
#include <chrono>
#include <functional>

Timer* Timer::instance = nullptr;
Timer* Timer::GetInstance() {
    if (!instance) {
        instance = new Timer();
        instance->Schedule();
    }
    return instance;
}

void Timer::Schedule() {
    m_timer_thread = std::thread(std::bind(&Timer::TimerLoop, this));
    m_timer_thread.detach();
}

void Timer::TimerLoop() {
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(TIMER_TICK_MS));
        std::lock_guard<std::mutex> lock(m_mutex);

        // auto now_ms = CurrentTimeInMilliSecond() / TIMER_TICK_MS;
        uint32_t timer_id = 0;
        uint8_t current_wheel_slot = this->m_current_time & TIMER_MASK;
        uint8_t next_wheel_slot = current_wheel_slot;
        for (uint8_t i = 0; i < TIMER_WHEELS - 1 && next_wheel_slot == 0; i++) {
            next_wheel_slot =
                (this->m_current_time >> ((i + 1) * TIMER_WHEEL_SLOT_BITS)) & TIMER_MASK;
            CascadeTimer(i + 1, next_wheel_slot);
        }

        TimerTask* pTimerTask = this->head[0][current_wheel_slot];
        TimerTask* next = nullptr;
        for (; pTimerTask != nullptr; pTimerTask = next) {
            next = pTimerTask->next;
            timer_id = pTimerTask->timer_id;
            if (!pTimerTask->canceled) {
                pTimerTask->callback(pTimerTask->timer_id, pTimerTask->args);
                if (pTimerTask->is_period) {
                    pTimerTask->expire = this->m_current_time + pTimerTask->interval;
                    AddTimerTask(pTimerTask, 0, current_wheel_slot);
                } else {
                    DelTimerTask(pTimerTask);  // 执行完删除定时器
                }
            } else {
                DelTimerTask(pTimerTask);  // 任务由用户取消，删除定时器
            }
        }
        this->head[0][current_wheel_slot] = nullptr;
        this->tail[0][current_wheel_slot] = nullptr;
        this->m_current_time++;  // 更新当前时间
    }
}

Timer::Timer() : m_timer_id(0) {
    for (int8_t wi = 0; wi < TIMER_WHEELS; wi++) {
        for (int8_t si = 0; si < TIMER_WHEEL_SLOTS; si++) {
            this->head[wi][si] = nullptr;
            this->tail[wi][si] = nullptr;
        }
    }
    // 当前时间，时间精度为 TIMER_TICK_MS 毫秒，默认 50ms 的时间精度
    this->m_current_time = CurrentTimeInMilliSecond() / TIMER_TICK_MS;
}

Timer::~Timer() {
    for (uint8_t wi = 0; wi < TIMER_WHEELS; wi++) {
        for (uint8_t si = 0; si < TIMER_WHEEL_SLOTS; si++) {
            TimerTask* pTimerTask = this->head[wi][si];
            TimerTask* next = nullptr;
            for (; pTimerTask != nullptr; pTimerTask = next) {
                next = pTimerTask->next;
                delete pTimerTask;
            }
        }
    }
}

// 获取当前时间，以毫秒为单位
int64_t Timer::CurrentTimeInMilliSecond() {
    std::chrono::high_resolution_clock::time_point now =
        std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto milli_seconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return milli_seconds;
}

std::unordered_map<uint32_t, TimerTask*> Timer::GetTimerMap() {
    return m_timer_map;
}

/**
 * @todo 根据定时器ID，删除对应定时器
 * @param timer_id 定时器ID
 */
bool Timer::CancelTimer(uint32_t timer_id) {
    Timer* pTimer = Timer::GetInstance();
    return pTimer->CancelTimerInternal(timer_id);
}

bool Timer::CancelTimerInternal(uint32_t timer_id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::unordered_map<uint32_t, TimerTask*>::iterator it = m_timer_map.find(timer_id);
    if (it == m_timer_map.end()) {
        return false;
    }

    TimerTask* pTimerTask = it->second;
    if (pTimerTask->canceled) {
        return false;
    }

    pTimerTask->canceled = true;
    m_timer_map.erase(it);
    return true;
}

/**
 * @todo
 * 删除所有定时器，设置定时任务状态为取消，让时间轮执行到对应定时任务时，直接跳过即可
 */
void Timer::CancelAllTimer() {
    Timer* pTimer = Timer::GetInstance();
    pTimer->CancelAllTimerInternal();
}

void Timer::CancelAllTimerInternal() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (std::unordered_map<uint32_t, TimerTask*>::iterator it = m_timer_map.begin();
         it != m_timer_map.end(); ++it) {
        it->second->canceled = true;
    }
    m_timer_map.clear();
}

uint32_t Timer::GetTimerId() {
    return m_timer_id;
}

void Timer::IncrementTimerId() {
    m_timer_id += 1;
}

int64_t Timer::CurrentTime() {
    return m_current_time;
}

/**
 * @todo 设置超时任务
 * @param timeout, 超时时间，单位ms
 * @param callback, 定时任务回调函数
 * @param arguments, 传递给回调函数 callback 的参数
 */
uint32_t Timer::SetTimeout(int timeout,
                           void (*callback)(uint32_t timer_id, void* args),
                           void* arguments) {
    Timer* pTimer = Timer::GetInstance();
    return pTimer->AddTimerTask(true, 0, callback, timeout, arguments);
}

/**
 * @todo 设置超时任务
 * @param interval, 间隔时间，单位ms
 * @param callback, 定时任务回调函数
 * @param delay_time, 延迟执行时间
 * @param arguments, 传递给回调函数 callback 的参数
 */
uint32_t Timer::SetInterval(int interval,
                            void (*callback)(uint32_t timer_id, void* args),
                            int delay_time,
                            void* arguments) {
    Timer* pTimer = Timer::GetInstance();
    return pTimer->AddTimerTask(true, interval, callback, delay_time, arguments);
}

uint32_t Timer::AddTimerTask(bool is_period,
                             int interval,
                             void (*callback)(uint32_t timer_id, void* args),
                             int delay_time,
                             void* arguments) {
    if (callback == nullptr) {
        return TIMER_CREATE_FAILED;
    }

    if (interval <= 0 || interval < TIMER_TICK_MS) {
        return TIMER_CREATE_FAILED;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    TimerTask* new_timer_task = new TimerTask();
    if (new_timer_task == nullptr) {
        return TIMER_CREATE_FAILED;
    }

    interval /= TIMER_TICK_MS;
    delay_time /= TIMER_TICK_MS;
    IncrementTimerId();
    new_timer_task->timer_id = GetTimerId();
    new_timer_task->is_period = is_period;
    new_timer_task->callback = callback;
    new_timer_task->args = arguments;
    new_timer_task->canceled = false;
    new_timer_task->expire = CurrentTime() + delay_time;
    if (is_period) {
        new_timer_task->interval = CurrentTime() + interval;
    } else {
        new_timer_task->interval = 0;
    }

    AddTimerTask(new_timer_task, 0, 0);
    return GetTimerId();
}

void Timer::AddTimerTask(TimerTask* pTimerTask, uint8_t wheel_index, uint8_t slot_index) {
    uint8_t wi = 0;
    uint8_t si = 0;

    uint32_t expire = pTimerTask->expire;
    uint32_t timeout = expire - m_current_time;

    if (timeout < uint32_t(1) << (TIMER_WHEEL_SLOT_BITS * (TIMER_WHEELS))) {
        for (uint32_t i = 0; i < TIMER_WHEELS; i++) {
            if (timeout < uint32_t(1) << (TIMER_WHEEL_SLOT_BITS * (i + 1))) {
                si = (expire >> (TIMER_WHEEL_SLOT_BITS * i)) & TIMER_MASK;
                wi = i;
                break;
            }
        }
    } else if (timeout < 0) {
        wi = 0;
        si = m_current_time & TIMER_MASK;
    } else {
        return;  // 不支持添加超时时间过大的定时器
    }

    if (tail[wi][si] == nullptr) {
        head[wi][si] = pTimerTask;
        tail[wi][si] = pTimerTask;
        pTimerTask->next = nullptr;
    } else {
        tail[wi][si]->next = pTimerTask;
        pTimerTask->next = nullptr;
        tail[wi][si] = pTimerTask;
    }
}

// 循环该slot里的所有节点，重新AddTimer到管理器中
void Timer::CascadeTimer(uint32_t wi, uint32_t si) {
    TimerTask* pTimerTask = head[wi][si];
    TimerTask* next = nullptr;
    for (; pTimerTask != nullptr; pTimerTask = next) {
        next = pTimerTask->next;
        if (!pTimerTask->canceled) {
            AddTimerTask(pTimerTask, wi, si);
        } else {
            DelTimerTask(pTimerTask);
        }
    }
    head[wi][si] = nullptr;
    tail[wi][si] = nullptr;
}

void Timer::DelTimerTask(TimerTask* pTimerTask) {
    this->m_timer_map.erase(pTimerTask->timer_id);
    delete pTimerTask;
}