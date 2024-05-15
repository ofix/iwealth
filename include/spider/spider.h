#pragma once
#include <string>
#include "net/request.h"

class ShareDataCenter;
class Spider {
   public:
    Spider(ShareDataCenter* data_center);
    std::string Fetch(const std::string& url);
    virtual ~Spider();
    void Run();
    void Stop();
    void Pause();
    bool HasFinish();
    double GetProgress();

   protected:
    ShareDataCenter* m_pDataCenter;
    double m_progress;
};
