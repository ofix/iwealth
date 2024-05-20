#include "stock/HashProvinceShare.h"

void HashProvinceShare::Insert(const std::string& province, Share* share) {
    if (m_map.find(province) != m_map.end()) {
        m_map[province].push_back(share);
    } else {
        std::vector<Share*> shares;
        shares.push_back(share);
        m_map[province] = shares;
    }
}

std::vector<Share*> HashProvinceShare::Query(const std::string& province) {
    if (m_map.find(province) != m_map.end()) {
        return m_map[province];
    }
    return {};
}