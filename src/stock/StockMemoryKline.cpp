#include "stock/StockMemoryKline.h"

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