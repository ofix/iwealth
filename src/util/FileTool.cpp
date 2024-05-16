///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/util/file_tool.cpp
// Purpose:     GUI main frame
// Author:      songhuabiao
// Modified by:
// Created:     2024-05-16-09.33
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "util/FileTool.h"

#ifdef _WIN32
#include <direct.h>  // _mkdir
#include <windows.h>
#else
#include <errno.h>  // errno, ENOENT, EEXIST
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

bool FileTool::IsFileExists(const std::string& path) {
#ifdef _WIN32
    struct _stat info;
    if (_stat(path.c_str(), &info) == 0) {
        return true;
    }
    return false;
#else
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return true;
    }
    return false;
#endif
}

/**
 * @todo 获取当前可执行文件所在目录文件路径
 * @author songhuabiao@iwealth.com.cn
 */
std::string FileTool::CurrentPath() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
    }
#endif
    std::string path = ParentDir(buffer);
    return path;
}

std::string FileTool::ParentDir(char* path) {
    int len = strlen(path) - 1;
    for (int i = len; i > 0; i--) {
        if (path[i] == '/') {
            path[i + 1] = 0;
            break;
        }
    }
    std::string dir(path);
    return dir;
}

// 加载整个文件到内存
std::string FileTool::LoadFile(const std::string& file_path) {
    std::ifstream ifs{file_path.c_str(), std::ios::in | std::ios::binary | std::ios::ate};
    if (ifs.good()) {
        std::ifstream::pos_type file_size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char> bytes(file_size);
        ifs.read(bytes.data(), file_size);
        return std::string(bytes.data(), file_size);
    }
    return "";
}

std::vector<std::string> FileTool::LoadDir(const std::string& path,
                                           const std::string& file_suffix) {
    std::vector<std::string> result;
    return result;
}

// 保存文件
bool FileTool::SaveFile(const std::string& file_path, const std::string& content) {
    std::ofstream out(file_path);
    out << content;
    out.close();
    return true;
}

// 递归创建文件夹
bool FileTool::MakeDirs(const std::string& path) {
#ifdef _WIN32
    int ret = _mkdir(path.c_str());
#else
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
#endif

    if (ret == 0) {
        return true;
    }

    switch (errno) {
        case ENOENT: {
            size_t pos = path.find_last_of('/');
            if (pos == std::string::npos)
#ifdef _WIN32
                pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
                return false;
            if (!MakeDirs(path.substr(0, pos)))
                return false;
        }
#ifdef _WIN32
            return 0 == _mkdir(path.c_str());
#else
            return 0 == mkdir(path.c_str(), mode);
#endif

        case EEXIST:
            return IsFileExists(path);

        default:
            return false;
    }
}

std::regex FileTool::BuildRegexFromMap(const std::map<std::string, std::string>& map) {
    std::string pattern_str = "(";
    auto it = map.begin();
    if (it != map.end()) {
        pattern_str += it->first;
        for (++it; it != map.end(); ++it)
            pattern_str += "|" + it->first;
    }
    pattern_str += ")";
    return std::regex(pattern_str);
}

std::string FileTool::MultiRegexReplace(
    const std::string& text,
    const std::map<std::string, std::string>& replacement_map) {
    auto regex = BuildRegexFromMap(replacement_map);
    std::string result;
    std::sregex_iterator it(text.begin(), text.end(), regex);
    std::sregex_iterator end;

    size_t last_pos = 0;
    for (; it != end; ++it) {
        result += text.substr(last_pos, it->position() - last_pos);
        result += replacement_map.at(it->str());
        last_pos = it->position() + it->length();
    }
    result += text.substr(last_pos, text.size() - last_pos);

    return result;
}
