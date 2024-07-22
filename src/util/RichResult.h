#ifndef RICH_RESULT_H
#define RICH_RESULT_H

#include <string>

enum class RichStatus {
    Ok = 1,
    PARAMETER_ERROR,  // 参数错误
    NETWORK_ERROR,    // 网络错误
    PARSE_ERROR,      // 数据解析错误
    FILE_NOT_FOUND,   // 文件不存在
    FILE_EXPIRED,     // 文件数据过期
    FILE_DIRTY,       // 文件内容被污染了
    IO_DENY,          // 文件拒绝写入
    WRITE_FAILURE,    // 文件写入失败
};

struct RichResult {
   public:
    RichResult(RichStatus status, std::string desc);
    RichStatus status;
    std::string desc;

    bool Ok();
};

RichResult Error(RichStatus status, std::string desc = "");
RichResult Success();

#endif