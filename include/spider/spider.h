#pragma once
#include <string>
#include "request.h"

typedef struct _stock_item {
    std::string code;
    std::string name;
} StockItem;

class Spider {
   public:
    Spider();
    std::string Fetch(const std::string& url);
    virtual ~Spider();
    void Run();
    void Stop();
    void Pause();
    bool HasFinish();
    double GetProgress();

   protected:
    double m_progress;
    bool m_finished;
};