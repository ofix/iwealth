#include "stock_list_spider_hexun.h"

StockListSpiderHexun::StockListSpiderHexun(const std::string& save_path)
    : m_stock_list_file(save_path) {}

StockListSpiderHexun::~StockListSpiderHexun() {}

void StockListSpiderHexun::Run() {
    FetchStockListData(1);  //沪市A股
    m_progress = 0.25;
    FetchStockListData(2);  //深市A股
    m_progress = 0.50;
    FetchStockListData(6);  //创业板
    m_progress = 0.75;
    FetchStockListData(1789);  // 科创板
    m_progress = 1;
}

void StockListSpiderHexun::FetchStockListData(int market) {
    static std::map<int, std::string> kv = {
        {1, "sha"}, {2, "sza"}, {6, "cyb"}, {1789, "kcb"}};
    std::string url = "https://stocksquote.hexun.com/a/sortlist";
    std::string url_sh = url + "?block=" + std::to_string(market) +
                         "&title=15&direction=0&start=0&number=10000&column=code,name";
    std::string data = Fetch(url_sh);
    ParseStockListData(data, kv.at(market));
}

void StockListSpiderHexun::ParseStockListData(std::string& data, std::string market) {
    data = regex_replace(data, std::regex{R"(\()"}, "");
    data = regex_replace(data, std::regex{R"(\);)"}, "");
    std::cout << data << std::endl;
    // json o = json::parse(data);
}
