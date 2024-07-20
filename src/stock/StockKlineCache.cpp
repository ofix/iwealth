#include "stock/StockKlineCache.h"

StockKlineCache::StockKlineCache() {
}

StockKlineCache::~StockKlineCache() {
}

RichResult StockKlineCache::FetchDayKline(const std::string& share_code, std::vector<uiKline>& day_klines) {
    return Success();
}

RichResult StockKlineCache::FetchMinuteKline(const std::string& share_code, std::vector<minuteKline>& minute_klines) {
    return Success();
}

RichResult StockKlineCache::FetchFiveDayMinuteKline(const std::string& share_code,
                                                    std::vector<minuteKline>& fiveday_klines) {
    return Success();
}

RichResult StockKlineCache::LoadLocalDayKline(const std::string file_path, std::vector<uiKline>& day_klines) {
    return Success();
}
RichResult StockKlineCache::LoadLocalMinuteKline(const std::string file_path, std::vector<uiKline>& minute_klines) {
    return Success();
}
RichResult StockKlineCache::LoadLocalFiveDayMinuteKline(const std::string file_path,
                                                        std::vector<uiKline>& fiveday_klines) {
    return Success();
}
RichResult StockKlineCache::IsLocalFileExist(const std::string file_path) {
    return Success();
}
RichResult StockKlineCache::IsLocalFileExpired(const std::string file_path) {
    return Success();
}
std::string StockKlineCache::GetFilePathOfDayKline(const std::string& share_code) {
    return "";
}
