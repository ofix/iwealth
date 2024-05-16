#include "spider/Spider.h"
#include "stock/StockDataStorage.h"

Spider::Spider(StockDataStorage* storage)
    : m_pStockStorage(storage),
      m_posStart(0),
      m_posEnd(0),
      m_concurrentMode(false),
      m_timeStart(std::chrono::milliseconds(0)),
      m_timeEnd(std::chrono::milliseconds(0)),
      m_timeConsume(0),
      m_progress(0) {}

Spider::~Spider() {}

std::string Spider::Fetch(const std::string& url) {
    return HttpGet(url);
}

void Spider::Crawl() {
    m_timeStart = std::chrono::high_resolution_clock::now();
    DoCrawl();
    m_timeEnd = std::chrono::high_resolution_clock::now();
    m_timeConsume =
        std::chrono::duration_cast<std::chrono::milliseconds>(m_timeEnd - m_timeStart);
}

void Spider::DoCrawl() {}

void Spider::Pause() {}

void Spider::Stop() {}

bool Spider::IsConcurrentMode() const {
    return this->m_concurrentMode;
}

std::chrono::milliseconds Spider::GetTimeConsumed() const {
    return this->m_timeConsume;
}

bool Spider::HasFinish() {
    return m_progress >= 1.0;
}

double Spider::GetProgress() {
    return m_progress;
}
