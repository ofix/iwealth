#ifndef STOCK_MEMORY_KLINE_H
#define STOCK_MEMORY_KLINE_H

#include <queue>
#include <string>
#include <unordered_map>
#include "stock/Stock.h"

template <typename T>
class StockMemoryKline {
   public:
    StockMemoryKline(int capacity_size = 300);
    virtual ~StockMemoryKline();
    void SetCapacitySize(int capacity_size);
    void Add(const std::string& share_code, const std::vector<T>& klines);
    std::vector<T>* Query(const std::string& share_code);

   private:
    int m_capacity_size;
    std::queue<std::string> m_klines_queue;
    std::unordered_map<std::string, std::vector<T>> m_klines_map;
};

// 模板类实现和声明必须放在同一个文件中，否则会出现链接错误！

template <typename T>
StockMemoryKline<T>::StockMemoryKline(int capacity_size) : m_capacity_size(capacity_size) {
}

template <typename T>
StockMemoryKline<T>::~StockMemoryKline() {
}

template <typename T>
void StockMemoryKline<T>::SetCapacitySize(int capacity_size) {
    m_capacity_size = capacity_size;
}

template <typename T>
void StockMemoryKline<T>::Add(const std::string& share_code, const std::vector<T>& klines) {
    // 如果超过容量限制，先移除最老的元素
    if (m_klines_queue.size() >= m_capacity_size) {
        std::string share_code = m_klines_queue.front();
        m_klines_map.erase(share_code);
        m_klines_queue.pop();
    }
    m_klines_queue.emplace(share_code);
    m_klines_map[share_code] = klines;
}

template <typename T>
std::vector<T>* StockMemoryKline<T>::Query(const std::string& share_code) {
    if (m_klines_map.find(share_code) != m_klines_map.end()) {
        return &m_klines_map[share_code];
    }
    return nullptr;
}

#endif