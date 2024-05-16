#pragma once
#include "spider/Spider.h"

class StockDataStorage;
class SpiderShareHistory : public Spider {
   public:
    SpiderShareHistory(StockDataStorage* storage);
    virtual ~SpiderShareHistory();
};
