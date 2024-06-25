#include <string>
#include <vector>
#include "search/ChinesePinYin.h"
#include "search/LevelTree.hpp"
#include "search/Trie.h"
#include "spider/SpiderConceptListEastMoney.h"
#include "spider/SpiderShareCategory.h"
#include "spider/SpiderShareKline.h"
#include "spider/SpiderShareQuote.h"
#include "stock/Stock.h"
#include "stock/StockDataStorage.h"
#include "util/DateTime.h"
#include "util/FileTool.h"
#include "util/Global.h"
#include "util/Timer.h"

template <typename N, typename L>
void InsertTree(const std::string& str, LevelTree<N, L>& tree) {
    std::vector<std::string> result = split(str, ",");
    std::vector<std::string> non_leaf_path = split(result[0], "-");

    std::string share_name = result[result.size() - 1];

    L* pShare = new L();
    pShare->name = share_name;

    tree.Insert(non_leaf_path, pShare);
}

std::vector<std::string> GetNonLeafTreeNode(const std::string& str) {
    std::vector<std::string> result = split(str, ",");
    std::vector<std::string> non_leaf_path = split(result[0], "-");
    return non_leaf_path;
}

Share* GetShare(const std::string& share_name) {
    Share* pShare = new Share();
    pShare->name = share_name;
    return pShare;
}

void TestLevelTree() {
    LevelTree<std::string, Share> tree;
    // std::vector<std::string> non_leafs =
    // GetNonLeafTreeNode("医药生物-生物制品-血液制品,上海莱士"); Share* share =
    // GetShare("上海莱士"); tree.Insert(non_leafs,share);
    InsertTree("医药生物-生物制品-血液制品,上海莱士", tree);
    InsertTree("商业贸易-零售-商业物业经营,小商品城", tree);
    InsertTree("机械设备-通用设备-其他专用设备,中亚股份", tree);
    InsertTree("房地产-房地产开发-住宅开发,合肥城建", tree);
    InsertTree("医药生物-生物制品-血液制品,天坛生物", tree);
    InsertTree("医药生物-生物制品-血液制品,华兰生物", tree);
    tree.Print();
}

void TestSpiderConceptListEastMoney() {
    StockDataStorage* m_pStockStorage = new StockDataStorage();
    m_pStockStorage->Init();
    SpiderConceptListEastMoney* spiderEastMoney = new SpiderConceptListEastMoney(m_pStockStorage);
    spiderEastMoney->Crawl();
}

void TestSpiderShareKline() {
    StockDataStorage* m_pStockStorage = new StockDataStorage();
    m_pStockStorage->Init();
    SpiderShareKline* spiderKline = new SpiderShareKline(m_pStockStorage, true);
    spiderKline->Crawl(KlineType::Day);
}

void TestSpiderShareCategory() {
    StockDataStorage* m_pStockStorage = new StockDataStorage();
    m_pStockStorage->Init();
    SpiderShareCategory* spiderShareLabel = new SpiderShareCategory(m_pStockStorage, true);
    spiderShareLabel->Crawl();
}

void TestSpiderShareHexun() {
    StockDataStorage* m_pStockStorage = new StockDataStorage();
    m_pStockStorage->Init();
    SpiderShareQuote* spiderHexun = new SpiderShareQuote(m_pStockStorage);
    spiderHexun->Crawl();
}

void TestTimer() {
    // 添加一次性定时器
    Timer::SetTimeout(2000, [](uint32_t timer_id, void* /*args*/) {
        std::cout << "Timer[" << timer_id << "], timeout: 2s, task count: " << Timer::GetTaskCount() << std::endl;
        Timer::SetTimeout(1000, [](uint32_t timer_id, void* /*args*/) {
            std::cout << "Timer[" << timer_id << "], timeout: 3s, task count: " << Timer::GetTaskCount() << std::endl;
        });
        Timer::SetTimeout(2000, [](uint32_t timer_id, void* /*args*/) {
            std::cout << "Timer[" << timer_id << "], timeout: 4s, task count: " << Timer::GetTaskCount() << std::endl;
        });
    });

    // 添加周期性定时器
    Timer::SetInterval(1000, [](uint32_t timer_id, void* /*args*/) {
        std::cout << "Timer[" << timer_id << "], interval: 1s, task count: " << Timer::GetTaskCount() << std::endl;
        static int loop = 0;
        loop += 1;
        if (loop == 5000) {
            // 取消周期性定时任务
            Timer::CancelTimer(timer_id);
        }
    });
}

void KlineTest() {
    std::string error_kline =
        "1483459200,2017-01-04,6.84,6.85,5320285,6.86,6.77,37188319.00,-0.02,-0.29,--,6.87,6.83,4333075,6.77,"
        "4819666,6."
        "75,5817798";
    uiKline kline;
    bool result = SpiderShareKline::ParseKlineBaidu(error_kline, &kline);
    if (!result) {
        std::cout << "parse baidu finance kline error!" << std::endl;
    }
}

void TestDateTime() {
    bool result = between_time_period("09:00", "10:36");
    std::cout << "between_time_period " << result << std::endl;
    std::string year = now("%Y");
    std::string month = now("%Y-%m");
    std::string day = now("%Y-%m-%d");
    std::string hour = now("%Y-%m-%d %H");
    std::string minute = now("%Y-%m-%d %H:%M");
    std::string second = now("%Y-%m-%d %H:%M:%S");
    std::cout << "  year = " << year << std::endl;
    std::cout << " month = " << month << std::endl;
    std::cout << "   day = " << day << std::endl;
    std::cout << "  hour = " << hour << std::endl;
    std::cout << "minute = " << minute << std::endl;
    std::cout << "second = " << second << std::endl;
    std::string start_time = second;
    std::string end_time = now("%Y-%m-%d ") + "10:35:00";
    std::cout << start_time << "-" << end_time << " diff " << diff_seconds(start_time, end_time) << " seconds"
              << std::endl;
}

void TestFile() {
    std::string file_path = FileTool::CurrentPath() + "data" + DIRECTORY_SEPARATOR + "stock_quote.json";
    std::string modified_time = FileTool::GetFileModifiedTime(file_path);
    std::cout << "file modifed time = " + modified_time << std::endl;
}

void TestTrie() {
    Trie trie;
    trie.insert("上海莱士", "002252");
    trie.insert("SHLS", "002252");
    trie.insert("上海新阳", "300236");
    trie.insert("SHXY", "300236");
    trie.insert("TTSW", "600161");
    trie.insert("天坛生物", "600161");
    std::vector<std::string> shares = trie.listPrefixWith("T");
    for (auto& share : shares) {
        std::cout << share << std::endl;
    }
    std::unordered_map<std::string, std::vector<std::string>> result = trie.list();
    for (auto& share : result) {
        std::cout << "[" << share.first << "] = ";
        std::cout << join(share.second, ",") << std::endl;
    }
}

void TestChinesePinYin() {
    std::vector<std::string> share_names = {
        "上海莱士",
        "小商品城",
        "*ST商城",
    };
    for (auto& share_name : share_names) {
        std::cout << share_name << " -> ";
        std::vector<std::string> pinyin = ChinesePinYin::GetFirstLetters(share_name);
        for (auto& py : pinyin) {
            std::cout << py << ",";
        }
        std::cout << std::endl;
        std::cout << share_name << " -> ";
        std::vector<std::string> full_pinyin = ChinesePinYin::GetLetters(share_name);
        for (auto& py : full_pinyin) {
            std::cout << py << ",";
        }
        std::cout << std::endl;
    }
}

int main(int /*argc*/, char** /*argv*/) {
    // TestSpiderConceptListEastMoney();
    // TestTimer();
    // KlineTest();
    // TestSpiderShareKline();
    // TestSpiderShareHexun();
    // TestDateTime();
    // TestFile();
    // TestTrie();
    TestChinesePinYin();
    std::cin.get();
}
