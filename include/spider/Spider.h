#pragma once
#include <string>
#include "net/Request.h"

class StockDataStorage;
class Spider {
   public:
    Spider(StockDataStorage* storage);
    std::string Fetch(const std::string& url);
    virtual ~Spider();
    void Run();
    void Stop();
    void Pause();
    bool HasFinish();
    double GetProgress();

   protected:
    StockDataStorage* m_pStockStorage;
    double m_progress;
};
