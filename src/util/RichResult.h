#ifndef RICH_RESULT_H
#define RICH_RESULT_H

#include <string>

enum class RichStatus {
    Ok = 1,
    DATA_NOT_FOUND,       // 数据不存在
    REPEAT_INIT,          // 重复初始化
    PARAMETER_ERROR,      // 参数错误
    NETWORK_ERROR,        // 网络错误
    PARSE_ERROR,          // 数据解析错误
    MEMORY_ALLOC_FAILED,  // 内存分配失败
    SHARE_NOT_EXIST,      // 股票不存在
    FILE_NOT_FOUND,       // 文件不存在
    FILE_READ_FAILED,     // 文件读取失败
    FILE_EXPIRED,         // 文件数据过期
    FILE_DIRTY,           // 文件内容被污染了
    FILE_WRITE_DENY,      // 文件拒绝写入
    FILE_WRITE_FAILED,    // 文件写入失败
    INNER_ERROR,          // 内部错误
};

struct RichResult {
   public:
    RichResult(RichStatus status = RichStatus::Ok, std::string desc = "");
    RichStatus status;
    std::string desc;
    std::string What();
    bool Ok();
};

RichResult Error(RichStatus status, std::string desc = "");
RichResult Success();

#endif