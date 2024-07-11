#ifndef STOCK_MONITOR_SHARE_H
#define STOCK_MONITOR_SHARE_H

#include "Stock.h"
#include "util/Global.h"

std::string PATH_MONITOR_SHARE = std::string("data") + DIR_SEPARATOR + "monitor_shares.json";

class StockDataStorage;
class StockMonitorShare {
   public:
    StockMonitorShare(StockDataStorage* pStorage);
    virtual ~StockMonitorShare();
    // 添加股票到监控列表中
    bool AddShareToMonitorList(const std::string& share_code);
    // 从监控列表中删除股票
    bool DelShareFromMonitorList(const std::string& share_code);
    // 从本地文件中加载监控股票列表
    bool Load();
    // 保存监控股票列表到本地文件
    bool Save();

   protected:
    bool CalcShareChangeRate(const MonitorShare& share, std::vector<uiKline>& klines);
    std::vector<MonitorShare> m_monitor_shares;
    StockDataStorage* m_pStorage;
};

#endif