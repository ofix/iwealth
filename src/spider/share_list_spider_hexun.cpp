#include "spider/share_list_spider_hexun.h"
#include "stock/share_data_center.h"

using json = nlohmann::json;

ShareListSpiderHexun::ShareListSpiderHexun(ShareDataCenter* data_center)
    : Spider(data_center) {}

ShareListSpiderHexun::~ShareListSpiderHexun() {}

void ShareListSpiderHexun::Run() {
    FetchMarketShares(1);     // 沪市A股
    FetchMarketShares(2);     // 深市A股
    FetchMarketShares(6);     // 创业板
    FetchMarketShares(1789);  // 科创板
}

void ShareListSpiderHexun::FetchMarketShares(int market) {
    static std::map<int, Market> kv = {
        {1,Market::ShangHai}, {2,Market::ShenZhen}, {6,Market::ChuangYeBan}, {1789, Market::KeChuangBan}};
    std::string url = "https://stocksquote.hexun.com/a/sortlist";
    std::string url_sh = url + "?block=" + std::to_string(market) +
                         "&title=15&direction=0&start=0&number=10000&column=code,name";
    std::string data = Fetch(url_sh);
    ParseStockListData(data, kv.at(market));
}

void ShareListSpiderHexun::ParseStockListData(std::string& data, Market market) {
    data = regex_replace(data, std::regex{R"(\()"}, "");
    data = regex_replace(data, std::regex{R"(\);)"}, "");
    json o = json::parse(data);
    const int count = o["Total"].template get<int>();
    json arr = o["Data"][0];
    for (json::iterator it = arr.begin(); it != arr.end(); ++it) {
        Share share;
        share.code = (*it)[0].template get<std::string>();
        share.name = (*it)[1].template get<std::string>();
        share.market = market;
        m_pDataCenter->m_market_shares.push_back(share);
    }
    m_pDataCenter->m_market_share_count.insert({market, count});
    m_pDataCenter->m_market_share_total+=count;
}
