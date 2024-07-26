#ifndef STOCK_COMPOSITE_INDEX_H
#define STOCK_COMPOSITE_INDEX_H

#include <string>
#include <unordered_map>
#include <vector>
#include "stock/Stock.h"
#include "util/RichResult.h"

struct StockIndex {
    std::string index_name;               // 股指名称
    std::vector<std::string> share_list;  // 成分股股票代号列表
};

class StockDataStorage;
class StockCompositeIndex {
   public:
    StockCompositeIndex(StockDataStorage* pStorage);
    virtual ~StockCompositeIndex();
    // 查询所有股指的名称
    std::vector<std::string> QueryAllCompositeIndexNames();
    // 查询股指成分股信息
    StockIndex* QueryCompositeIndexInfo(const std::string& index_name);
    // 查询股指最近一个交易日的分时图数据
    RichResult QueryCompositeIndexMinuteKlines(const std::string& index_name, std::vector<minuteKline>& minute_klines);
    // 查询股指的日K线/周K线/月K线/季K线/年K线数据
    RichResult QueryCompositeIndexKlines(const KlineType kline_type,
                                         const std::string& index_name,
                                         std::vector<uiKline>& day_klines);

   protected:
    // 添加股指信息到缓存
    void AddCompositeIndexInfo(const std::string& index_name, const std::vector<std::string>& share_list);
    // 异步下载单个股指成分股数据
    RichResult FetchCompositeIndexInfo(const std::string& index_name);
    // 异步下载所有股指成分股数据
    RichResult FetchAllCompositeIndexInfo(const std::vector<std::string>& urls);
    // 异步下载单个股指行情数据
    RichResult FetchCompositeIndexQuote(const std::string& index_name);
    // 异步下载所有股指行情数据
    RichResult FetchAllCompositeIndexQuote(const std::vector<std::string>& urls);
    // 保存股指成分股数据
    bool SaveCompositeIndexInfo(const std::string& index_name);
    // 加载股指成分股数据
    bool LoadCompositeIndexInfo(const std::string& index_name, std::vector<StockIndex>& stock_index);
    // 保存股指日K线数据
    bool SaveCompositeIndexDayKlines(const std::string& index_name);
    // 加载股指日K线数据
    bool LoadCompositeIndexDayKlines(const std::string& index_name, std::vector<uiKline>& day_klines);

    std::vector<Share*> GetShareList(const std::string index_name);
    std::vector<Share*> GetShareList(const std::vector<std::string>& share_code_list);

   private:
    StockDataStorage* m_pStorage;
    std::vector<StockIndex> m_indexes;  // 上证指数/深圳指数/沪深300/中证100 等
    std::unordered_map<std::string, std::vector<Share*>> m_index_map;  // 缓存股票指数和股票指针的关系
};

#endif