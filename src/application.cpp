#include <iostream>
#include <string>
#include "spider/stock_list_spider_hexun.h"

int main(int argc, char** argv) {
    StockListSpiderHexun spider("");
    spider.Run();
    // std::string response = HttpGet("https://www.baidu.com");
    // std::cout << response << std::endl;
    return 1;
}
