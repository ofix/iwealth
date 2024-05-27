#include <string>
#include <vector>
#include "search/LevelTree.hpp"
#include "spider/SpiderConceptListEastMoney.h"
#include "spider/SpiderShareKline.h"
#include "stock/Stock.h"
#include "stock/StockDataStorage.h"
#include "util/Global.h"
#include "util/Timer.h"

template <typename N, typename L>
void InsertTree(const std::string& str, LevelTree<N, L>& tree) {
    std::vector<std::string> result = split(str, ',');
    std::vector<std::string> non_leaf_path = split(result[0], '-');

    std::string share_name = result[result.size() - 1];

    L* pShare = new L();
    pShare->name = share_name;

    tree.Insert(non_leaf_path, pShare);
}

std::vector<std::string> GetNonLeafTreeNode(const std::string& str) {
    std::vector<std::string> result = split(str, ',');
    std::vector<std::string> non_leaf_path = split(result[0], '-');
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

void TestSpiderShareHistoryBaidu() {
    StockDataStorage* m_pStockStorage = new StockDataStorage();
    m_pStockStorage->Init();
    SpiderShareKline* spiderBaidu = new SpiderShareKline(m_pStockStorage);
    spiderBaidu->SetCrawlRange(3, 3);
    spiderBaidu->Crawl(KlineType::Day);
}

void TestTimer() {
    // 添加一次性定时器
    Timer::SetTimeout(2000, [](uint32_t timer_id, void* args) {
        std::cout << "Timer[" << timer_id << "], timeout: 2s, task count: " << Timer::GetTaskCount()
                  << std::endl;
        Timer::SetTimeout(2000, [](uint32_t timer_id, void* args) {
            std::cout << "Timer[" << timer_id << "], timeout: 4s, task count: " << Timer::GetTaskCount()
                      << std::endl;
        });
        Timer::SetTimeout(8000, [](uint32_t timer_id, void* args) {
            std::cout << "Timer[" << timer_id << "], timeout: 8s, task count: " << Timer::GetTaskCount()
                      << std::endl;
        });
    });

    // 添加周期性定时器
    Timer::SetInterval(1000, [](uint32_t timer_id, void* args) {
        std::cout << "Timer[" << timer_id << "], interval: 1s, task count: " << Timer::GetTaskCount()
                  << std::endl;
        static int loop = 0;
        loop += 1;
        if (loop == 5000) {
            // 取消周期性定时任务
            Timer::CancelTimer(timer_id);
        }
    });
    std::cin.get();
}

int main(int argc, char* argv[]) {
    // TestSpiderConceptListEastMoney();
    // TestTimer();
    TestSpiderShareHistoryBaidu();
}