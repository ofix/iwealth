#pragma once
#include "spider/Spider.h"

class StockDataStorage;
class ShareHistorySpider : public Spider {
   public:
    ShareHistorySpider(StockDataStorage* storage);
    virtual ~ShareHistorySpider();
};
