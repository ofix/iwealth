#include "spider/SpiderBasicInfoEastMoney.h"
#include "stock/StockDataStorage.h"
#include "util/Global.h"

using json = nlohmann::json;

SpiderBasicInfoEastMoney::SpiderBasicInfoEastMoney(StockDataStorage* storage)
    : Spider(storage) {}

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
                                               {Market::ChuangYeBan, ".CYB"},
                                               {Market::KeChuangBan, ".KCB"}};
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
        "&filter=(SECUCODE=\"" +
        share.code + kv.at(share.market) + "\")";
    return url;
}

void SpiderBasicInfoEastMoney::ParseResponse(std::string& response, Share& share) {
    json _response = json::parse(response);
    json o = _response["result"]["data"][0];
    std::string old_names = o["FORMERNAME"].template get<std::string>();
    share.old_names = split(old_names, ',');
}

void SpiderBasicInfoEastMoney::ConcurrentFetchBasicInfo(size_t start_pos,
                                                        size_t end_pos) {
    std::vector<Share> shares = m_pStockStorage->m_market_shares;
    std::vector<std::string> urls;
    for (size_t i = start_pos; i < end_pos; i++) {
        urls.push_back(GetRequestUrl(shares[i]));
    }
}
