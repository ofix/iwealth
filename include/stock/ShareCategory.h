#ifndef ShareCategory_H
#define ShareCategory_H
#include <unordered_map>
#include <vector>
#include "Stock.h"

class ShareCategory {
   public:
    bool Insert(const std::string& category, Share* share);
    std::vector<Share*> Query(const std::string& category);
    void Clear();

   private:
    std::unordered_map<std::string, std::vector<Share*>> m_map;  // 省份映射表
};

#endif  // ShareCategory_H
