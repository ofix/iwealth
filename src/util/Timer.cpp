#include "util/Timer.h"
#include <chrono>

Timer* Timer::instance = nullptr;
Timer* Timer::GetInstance() {
    if (!instance) {
        instance = new Timer();
        instance->Schedule();
    }
    return instance;
}

void Timer::Schedule() {
    m_timer_thread = std::thread(std::bind(&Timer::Tick, this));
    m_timer_thread.detach();
}

void Timer::Tick() {
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(TIMER_TICK_MS));

        uint8_t current_wheel_slot = this->m_tick & TIMER_MASK;
        uint8_t next_wheel_slot = current_wheel_slot;
        for (uint8_t i = 0; i < TIMER_WHEELS - 1 && next_wheel_slot == 0;
             i++) {  // 最小轮转完一圈需要进位，将被动轮上的定时任务挪到主动轮上统一处理
            next_wheel_slot =
                (this->m_tick >> ((i + 1) * TIMER_WHEEL_SLOT_BITS)) & TIMER_MASK;
            MoveTimerTaskToTickWheel(i + 1, next_wheel_slot);
        }
        uint32_t timer_id = 0;
        TimerTask* pTimerTask = this->head[0][current_wheel_slot];
        TimerTask* next = nullptr;
        std::lock_guard<std::mutex> lock(link_list_mutex[0][current_wheel_slot]);
        for (; pTimerTask != nullptr; pTimerTask = next) {
            next = pTimerTask->next;
            timer_id = pTimerTask->timer_id;
            if (!pTimerTask->canceled) {
                pTimerTask->callback(pTimerTask->timer_id, pTimerTask->args);
                if (pTimerTask->is_period) {
                    pTimerTask->expire = this->m_tick + pTimerTask->interval;
                    InternalAddTimerTask(
                        pTimerTask);  // 重新添加定时任务，并不会增加定时任务数量，只是移动了指针
                } else {
                    DelTimerTask(pTimerTask);  // 执行完删除定时器
                }
            } else {
                DelTimerTask(pTimerTask);  // 任务由用户取消，删除定时器
            }
        }
        this->head[0][current_wheel_slot] = nullptr;
        this->tail[0][current_wheel_slot] = nullptr;
        this->m_tick++;  // 更新当前时间
    }
}

Timer::Timer() : m_timer_id(0), m_tick(0), m_timer_tasks(0) {
    for (int8_t wi = 0; wi < TIMER_WHEELS; wi++) {
        for (int8_t si = 0; si < TIMER_WHEEL_SLOTS; si++) {
            this->head[wi][si] = nullptr;
            this->tail[wi][si] = nullptr;
        }
    }
    // 当前时间，时间精度为 TIMER_TICK_MS 毫秒，默认 50ms 的时间精度
    // this->m_tick = 0;  // CurrentTimeInMilliSecond() / TIMER_TICK_MS;
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
    return pTimer->InternalCancelTimer(timer_id);
}

bool Timer::InternalCancelTimer(uint32_t timer_id) {
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
    pTimer->InternalCancelAllTimer();
}

void Timer::InternalCancelAllTimer() {
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

uint64_t Timer::CurrentTick() {
    return m_tick;
}

uint32_t Timer::GetTaskCount() {
    Timer* pTimer = Timer::GetInstance();
    return pTimer->InternalGetTaskCount();
}

uint32_t Timer::InternalGetTaskCount() {
    return m_timer_tasks;
}

uint32_t Timer::SetTimeout(int timeout,
                           std::function<void(uint32_t, void*)>& callback,
                           void* arguments) {
    Timer* pTimer = Timer::GetInstance();
#ifdef _WIN32
    return pTimer->AddTimerTask(false, 0, callback.target<void(uint32_t, void*)>(),
                                timeout, arguments);
#else  // 老版本GCC编译会报错，只好采用强制类型转换了
    return pTimer->AddTimerTask(false, 0, *(callback.target<void (*)(uint32_t, void*)>()),
                                timeout, arguments);
#endif
}

/**
 * @todo 设置超时任务
 * @param timeout, 超时时间，单位ms
 * @param callback, 定时任务回调函数
 * @param arguments, 传递给回调函数 callback 的参数
 */
uint32_t Timer::SetTimeout(uint32_t timeout,
                           void (*callback)(uint32_t timer_id, void* args),
                           void* arguments) {
    Timer* pTimer = Timer::GetInstance();
    return pTimer->AddTimerTask(false, 0, callback, timeout, arguments);
}

uint32_t Timer::SetInterval(uint32_t interval,
                            std::function<void(uint32_t, void*)>& callback,
                            uint32_t delay_time,
                            void* arguments) {
    Timer* pTimer = Timer::GetInstance();
#ifdef _WIN32
    return pTimer->AddTimerTask(true, interval,
                                callback.template target<void(uint32_t, void*)>(),
                                delay_time, arguments);
#else
    return pTimer->AddTimerTask(true, interval,
                                *(callback.target<void (*)(uint32_t, void*)>()),
                                delay_time, arguments);
#endif
}

/**
 * @todo 设置超时任务
 * @param interval, 间隔时间，单位ms
 * @param callback, 定时任务回调函数
 * @param delay_time, 延迟执行时间
 * @param arguments, 传递给回调函数 callback 的参数
 */
uint32_t Timer::SetInterval(uint32_t interval,
                            void (*callback)(uint32_t timer_id, void* args),
                            uint32_t delay_time,
                            void* arguments) {
    Timer* pTimer = Timer::GetInstance();
    return pTimer->AddTimerTask(true, interval, callback, delay_time, arguments);
}

uint32_t Timer::AddTimerTask(bool is_period,
                             uint32_t interval,
                             void (*callback)(uint32_t timer_id, void* args),
                             uint32_t delay_time,
                             void* arguments) {
    if (callback == nullptr) {
        return TIMER_CREATE_FAILED;
    }

    if (is_period && (interval <= 0 || interval < TIMER_TICK_MS)) {
        return TIMER_CREATE_FAILED;
    }

    interval /= TIMER_TICK_MS;
    delay_time /= TIMER_TICK_MS;

    if (is_period) {
        if (interval > uint32_t(1) << (TIMER_WHEEL_SLOT_BITS * (TIMER_WHEELS))) {
            return TIMER_CREATE_FAILED;
        }
    } else {
        if (delay_time > uint32_t(1) << (TIMER_WHEEL_SLOT_BITS * (TIMER_WHEELS))) {
            return TIMER_CREATE_FAILED;
        }
    }

    TimerTask* new_timer_task = new TimerTask();
    if (new_timer_task == nullptr) {
        return TIMER_CREATE_FAILED;
    }

    IncrementTimerId();
    new_timer_task->timer_id = GetTimerId();
    new_timer_task->is_period = is_period;
    new_timer_task->callback = callback;
    new_timer_task->args = arguments;
    new_timer_task->canceled = false;
    new_timer_task->expire = m_tick + delay_time;
    if (is_period) {
        new_timer_task->interval = m_tick + interval;
    } else {
        new_timer_task->interval = 0;
    }

    InternalAddTimerTask(new_timer_task);
    m_timer_tasks++;
    return GetTimerId();
}

void Timer::InternalAddTimerTask(TimerTask* pTimerTask) {
    uint8_t wi = 0;
    uint8_t si = 0;

    uint32_t expire = pTimerTask->expire;
    uint32_t ticks = expire - m_tick;

    for (uint32_t i = 0; i < TIMER_WHEELS; i++) {
        if (ticks < uint32_t(1) << (TIMER_WHEEL_SLOT_BITS * (i + 1))) {
            si = (expire >> (TIMER_WHEEL_SLOT_BITS * i)) & TIMER_MASK;
            wi = i;
            break;
        }
    }

    std::lock_guard<std::mutex> lock(link_list_mutex[wi][si]);

    if (tail[wi][si] == nullptr) {  // 空节点
        head[wi][si] = pTimerTask;
        tail[wi][si] = pTimerTask;
        pTimerTask->next = nullptr;
        pTimerTask->prev = head[wi][si];
    } else {  // 往尾部添加一个定时任务
        tail[wi][si]->next = pTimerTask;
        pTimerTask->next = nullptr;
        pTimerTask->prev = tail[wi][si];
        tail[wi][si] = pTimerTask;
    }
}

// 将其他轮子上的任务列表添加到主轮上
void Timer::MoveTimerTaskToTickWheel(uint32_t wi, uint32_t si) {
    TimerTask* pTimerTask = head[wi][si];
    TimerTask* next = nullptr;
    for (; pTimerTask != nullptr; pTimerTask = next) {
        next = pTimerTask->next;
        if (!pTimerTask->canceled) {
            InternalAddTimerTask(pTimerTask);
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
    m_timer_tasks--;
}