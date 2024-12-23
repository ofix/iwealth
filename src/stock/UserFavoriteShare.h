#ifndef USER_FAVORITE_SHARE_H
#define USER_FAVORITE_SHARE_H

#include "Stock.h"
#include "util/Global.h"

extern std::string DEFAULT_GROUP_NAME;
extern std::string PATH_USER_FAVORITE_SHARE;

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
    // 获取当前自选组名字
    std::string GetCurrentGroupName();
    // 获取自选组列表名字
    std::vector<std::string> GetGroupNames();
    std::vector<FavoriteShare> GetCurrentGroupShares();
    // 检查用户是否有自选股
    bool HasFavoriteGroup();
    // 获取自选组个数
    size_t GetGroupCount();
    // 设置当前自选组
    void SetCurrentGroup(size_t i);
    // 获取当前自选组列表大小
    size_t GetCurrentGroupSize();

   protected:
    // 根据股票自选日期，计算自选股加入自选后的涨跌幅,基于前复权的股票价格
    bool CalcShareChangeRate(FavoriteShare& favorite_share, std::vector<uiKline>* klines);
    bool CalcAllFavoriteSharesChangeRate();
    std::vector<FavoriteShareGroup> m_favoriteShareGroups;
    size_t m_currentGroup; // 当前选中的自选组
    StockDataStorage* m_pStorage;
};

#endif