#pragma once
#include "spider/spider.h"

class ShareDataCenter;
class ShareHistorySpider : public Spider {
   public:
    ShareHistorySpider(ShareDataCenter* data_center);
    virtual ~ShareHistorySpider();
};
