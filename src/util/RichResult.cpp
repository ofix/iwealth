#include "util/RichResult.h"

#include "util/RichResult.h"

RichResult::RichResult(RichStatus status, std::string desc) : status(status), desc(desc) {
}

bool RichResult::Ok() {
    return status == RichStatus::Ok;
}

std::string RichResult::What() {
    if (desc != "") {
        return desc;
    }
    if (status == RichStatus::FILE_WRITE_FAILED) {
        return "文件写入失败";
    } else if (status == RichStatus::FILE_EXPIRED) {
        return "本地数据已过期";
    } else if (status == RichStatus::FILE_DIRTY) {
        return "数据文件已损坏";
    } else if (status == RichStatus::FILE_READ_FAILED) {
        return "文件读取失败";
    } else if (status == RichStatus::FILE_NOT_FOUND) {
        return "文件不存在";
    } else if (status == RichStatus::FILE_WRITE_DENY) {
        return "获取文件写入权限失败";
    } else if (status == RichStatus::INNER_ERROR) {
        return "系统内部错误";
    } else if (status == RichStatus::MEMORY_ALLOC_FAILED) {
        return "内存空间不足";
    } else if (status == RichStatus::NETWORK_ERROR) {
        return "网络连接失败";
    } else if (status == RichStatus::PARAMETER_ERROR) {
        return "参数传入错误";
    } else if (status == RichStatus::PARSE_ERROR) {
        return "文件解析错误";
    } else if (status == RichStatus::SHARE_NOT_EXIST) {
        return "股票不存在";
    }
    return "";
}

RichResult Error(RichStatus status, std::string desc) {
    RichResult result(status, desc);
    return result;
}

RichResult Success() {
    RichResult result(RichStatus::Ok, "");
    return result;
}