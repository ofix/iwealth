#include "spider/SpiderBasicInfoEastMoney.h"
#include "net/ConcurrentRequest.h"
#include "stock/StockDataStorage.h"
#include "util/Global.h"

using json = nlohmann::json;

SpiderBasicInfoEastMoney::SpiderBasicInfoEastMoney(StockDataStorage* storage)
    : Spider(storage) {}

SpiderBasicInfoEastMoney::SpiderBasicInfoEastMoney(StockDataStorage* storage,
                                                   bool concurrent)
    : Spider(storage, concurrent) {}

SpiderBasicInfoEastMoney::~SpiderBasicInfoEastMoney() {}

void SpiderBasicInfoEastMoney::DoCrawl() {
    if (this->IsConcurrentMode()) {
        ConcurrentFetchBasicInfo(m_posStart, m_posEnd);
    } else {
        std::vector<Share> shares = m_pStockStorage->m_market_shares;
        for (size_t i = m_posStart; i < m_posEnd; i++) {
            Share share = shares[i];
            FetchBasicInfo(share);
        }
    }
}

void SpiderBasicInfoEastMoney::FetchBasicInfo(Share& share) {
    std::string url = GetRequestUrl(share);
    std::string data = Fetch(url);
    ParseResponse(data, share);
}

std::string SpiderBasicInfoEastMoney::GetRequestUrl(const Share& share) {
    static std::map<Market, std::string> kv = {{Market::ShangHai, ".SH"},
                                               {Market::ShenZhen, ".SZ"},
                                               {Market::ChuangYeBan, ".SZ"},
                                               {Market::KeChuangBan, ".SH"}};
    std::string url =
        "https://datacenter.eastmoney.com/securities/api/data/v1/get?"
        "reportName=RPT_F10_BASIC_ORGINFO"
        "&columns=SECUCODE,"
        "SECURITY_CODE,"
        "SECURITY_NAME_ABBR,"
        "EM2016,"
        "INDUSTRYCSRC1,"
        "PROVINCE,"
        "REG_CAPITAL,"
        "FORMERNAME,"
        "REG_NUM,"
        "EMP_NUM"
        "&pageNumber=1"
        "&pageSize=1"
        "&source=HSF10"
        "&client=PC"
        "&filter=(SECUCODE=%22" +
        share.code + kv.at(share.market) + "%22)";
    return url;
}

void SpiderBasicInfoEastMoney::ParseResponse(std::string& response, Share& share) {
    json _response = json::parse(response);
    json o = _response["result"]["data"][0];
    if (!o["FORMERNAME"].is_null()) {
        std::string old_names = o["FORMERNAME"].template get<std::string>();
        share.old_names = split(old_names, ',');
    }
}

void SpiderBasicInfoEastMoney::ConcurrentFetchBasicInfo(size_t start_pos,
                                                        size_t end_pos) {
    std::vector<Share> shares = m_pStockStorage->m_market_shares;
    std::vector<std::string> urls;
    for (size_t i = start_pos; i < end_pos; i++) {
        urls.push_back(GetRequestUrl(shares[i]));
    }
    std::function<void(std::string&)> callback =
        std::bind(&SpiderBasicInfoEastMoney::ConcurrentResponseCallback, this,
                  std::placeholders::_1);
    try {
        HttpConcurrentGet(urls, callback, 3);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void SpiderBasicInfoEastMoney::ConcurrentResponseCallback(std::string& response) {
    std::cout << "++++++  Concurrent Response Callback  +++++" << std::endl;
    std::cout << response << std::endl << std::endl;
}
