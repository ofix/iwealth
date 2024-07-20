#include "util/RichResult.h"

#include "util/RichResult.h"

RichResult::RichResult(RichStatus status, std::string desc) : status(status), desc(desc) {
}

bool RichResult::Ok() {
    return status == RichStatus::Ok;
}

RichResult Error(RichStatus status, std::string desc) {
    RichResult result(status, desc);
    return result;
}

RichResult Success() {
    RichResult result(RichStatus::Ok, "");
    return result;
}