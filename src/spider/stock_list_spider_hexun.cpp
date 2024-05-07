#include "stock_list_spider_hexun.h"
using json = nlohmann::json;

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
    std::vector<StockItem> all_stock_items = GetAllStockItems();
    PrintAllStockItems(all_stock_items);
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
    json o = json::parse(data);
    const int count = o["Total"].template get<int>();
    json arr = o["Data"][0];
    std::vector<StockItem> market_stocks;
    for (json::iterator it = arr.begin(); it != arr.end(); ++it) {
        StockItem item;
        item.code = (*it)[0].template get<std::string>();
        item.name = (*it)[1].template get<std::string>();
        market_stocks.push_back(item);
    }
    m_market_stock_count.insert({market, count});
    m_stock_list.insert({market, market_stocks});
}

std::vector<StockItem> StockListSpiderHexun::GetAllStockItems() {
    std::vector<StockItem> all_stocks;
    for (std::map<std::string, std::vector<StockItem>>::iterator it =
             m_stock_list.begin();
         it != m_stock_list.end(); it++) {
        std::vector<StockItem> stocks = it->second;
        all_stocks.insert(all_stocks.end(), stocks.begin(), stocks.end());
    }
    return all_stocks;
}

void StockListSpiderHexun::PrintAllStockItems(std::vector<StockItem>& all_stock_items) {
    json result = json::array();
    for (auto item : all_stock_items) {
        json o = json::object();
        o["code"] = item.code;
        o["name"] = item.name;
        result.push_back(o);
    }
    // json result(all_stock_items);
    std::string data = result.dump(4);
    std::cout << data << std::endl;
}