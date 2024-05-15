#include "spider/share_history_spider.h"
#include "stock/share_data_center.h"

ShareHistorySpider::ShareHistorySpider(ShareDataCenter* data_center)
    : Spider(data_center) {}

ShareHistorySpider::~ShareHistorySpider() {}
