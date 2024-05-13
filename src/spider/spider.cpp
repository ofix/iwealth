#include "spider/spider.h"

Spider::Spider() : m_finished(false) {}

Spider::~Spider() {}

std::string Spider::Fetch(const std::string& url) {
    return HttpGet(url);
}

void Spider::Run() {}

void Spider::Pause() {}

void Spider::Stop() {}

bool Spider::HasFinish() {
    return m_finished;
}

double Spider::GetProgress() {
    return m_progress;
}
