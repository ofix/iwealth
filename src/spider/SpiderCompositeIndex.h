#ifndef SPIDER_COMPOSITE_INDEX_H
#define SPIDER_COMPOSITE_INDEX_H

#include "spider/Spider.h"

class StockDataStorage;
class SpiderCompositeIndex : public Spider {
   public:
    SpiderCompositeIndex(StockDataStorage* pStorage);
    virtual ~SpiderCompositeIndex();

    void Crawl();      // 异步爬取
    bool CrawlSync();  // 同步爬取
    std::vector<std::string> Parse(const std::string& response);

   private:
};

#endif