#ifndef HashProvinceShare_H
#define HashProvinceShare_H
#include <unordered_map>
#include <vector>
#include "Stock.h"

class HashProvinceShare {
   public:
    void Insert(const std::string& province, Share* share);
    std::vector<Share*> Query(const std::string& province);

   private:
    std::unordered_map<std::string, std::vector<Share*>> m_map;  // 省份映射表
};

#endif  // HashProvinceShare_H
