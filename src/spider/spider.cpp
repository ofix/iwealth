#include "spider/spider.h"
#include "stock/share_data_center.h"

Spider::Spider(ShareDataCenter* data_center)
    : m_pDataCenter(data_center), m_progress(0) {}

Spider::~Spider() {}

std::string Spider::Fetch(const std::string& url) {
    return HttpGet(url);
}

void Spider::Run() {}

void Spider::Pause() {}

void Spider::Stop() {}

bool Spider::HasFinish() {
    return m_progress >= 1.0;
}

double Spider::GetProgress() {
    return m_progress;
}
