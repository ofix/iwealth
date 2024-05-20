#include <string>
#include <vector>
#include "search/LevelTree.hpp"
#include "stock/Stock.h"
#include "util/Global.h"

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

int main(int argc, char* argv[]) {
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