#ifndef RICH_PNG_KLINE_H
#define RICH_PNG_KLINE_H

#include <string>
#include <vector>
#include "stock/StockDataStorage.h"

class RichPngKline {
   public:
    RichPngKline(StockDataStorage* pStorage,const std::string& png_file_path, int width=1600, int height=900,int margin=20);
    ~RichPngKline();
    void AddShare(const std::string& share_code);
    void AddShares(const std::vector<std::string>& share_list);
    bool Save();
    void DrawPng(wxDC* pDC,int n, std::vector<uiKline>* pKlines);
    double GetMinPrice(std::vector<uiKline>* pKlines);
    double GetMaxPrice(std::vector<uiKline>* pKlines);

   private:
    StockDataStorage* m_pStorage;
    std::string m_pngFilePath;
    int m_width;
    int m_height;
    int m_margin;
    std::vector<std::string> m_shareList;
};

#endif