#include "spider/ShareHistorySpider.h"
#include "stock/StockDataStorage.h"

ShareHistorySpider::ShareHistorySpider(StockDataStorage* storage) : Spider(storage) {}

ShareHistorySpider::~ShareHistorySpider() {}
