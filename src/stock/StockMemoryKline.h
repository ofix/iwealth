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

#endif