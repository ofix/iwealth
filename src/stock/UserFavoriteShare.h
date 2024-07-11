#ifndef USER_FAVORITE_SHARE_H
#define USER_FAVORITE_SHARE_H

#include "Stock.h"
#include "util/Global.h"

std::string DEFAULT_GROUP_NAME = "默认分组";
std::string PATH_USER_FAVORITE_SHARE = std::string("data") + DIR_SEPARATOR + "favorite_shares.json";

class StockDataStorage;
class UserFavoriteShare {
   public:
    // 如果用户没有强制分组，创建一个默认分组
    UserFavoriteShare(StockDataStorage* pStorage);
    virtual ~UserFavoriteShare();
    // 创建自选股分组
    bool CreateFavoriteShareGroup(const std::string& favorite_group_name);
    // 删除自选股分组
    bool RemoveFavoriteShareGroup(const std::string& favorite_group_name);
    // 添加股票到自选股分组中
    bool AddShareToFavoriteGroup(const std::string& share_code,
                                 std::string& favorite_group_name = DEFAULT_GROUP_NAME,
                                 std::string favorite_date = "");
    // 从自选股中删除股票
    bool DelShareFromFavoriteGroup(const std::string& share_code, std::string& favorite_group_name);
    // 加载本地有用户自选股文件，加载它
    bool Load();
    // 保存用户自选股到本地文件
    bool Save();
    // 检查用户是否有自选股
    bool HasFavoriteGroup();

   protected:
    // 根据股票自选日期，计算自选股加入自选后的涨跌幅,基于前复权的股票价格
    bool CalcShareChangeRate(FavoriteShare& favorite_share, std::vector<uiKline>* klines);
    bool CalcAllFavoriteSharesChangeRate();
    std::vector<FavoriteShareGroup> m_favoriteShareGroups;
    StockDataStorage* m_pStorage;
};

#endif