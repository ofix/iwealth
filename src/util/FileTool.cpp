///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/util/FileTool.cpp
// Purpose:     file related operations
// Author:      songhuabiao
// Modified by:
// Created:     2024-06-14 19:48
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "util/FileTool.h"

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>  // _mkdir
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <fstream>
#else
#include <dirent.h>
#include <errno.h>  // errno, ENOENT, EEXIST
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#endif
#include <cstdio>
#include <iostream>

bool FileTool::IsFileExists(const std::string& path) {
#if defined(_WIN32) || defined(_WIN64)
    // struct _stat info;
    // if (_stat(path.c_str(), &info) == 0) {
    //     return true;
    // }
    // return false;
    std::ifstream file(path);
    return file.good();
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
#if defined(_WIN32) || defined(_WIN64)
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

// 获取文件修改时间，支持跨平台
std::string FileTool::GetFileModifiedTime(const std::string& path) {
#if defined(_WIN32) || defined(_WIN64)
    HANDLE hFile =
        CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return "";
    }

    FILETIME modified_time;
    FILETIME local_modified_time;
    if (!GetFileTime(hFile, NULL, NULL, &modified_time)) {
        CloseHandle(hFile);
        return "";
    }

    SYSTEMTIME system_time;
    FileTimeToLocalFileTime(&modified_time, &local_modified_time);
    FileTimeToSystemTime(&local_modified_time, &system_time);

    char buffer[80];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", system_time.wYear, system_time.wMonth, system_time.wDay,
            system_time.wHour, system_time.wMinute, system_time.wSecond);

    CloseHandle(hFile);

    return std::string(buffer);
#else
    struct stat buf;
    if (stat(path.c_str(), &buf) == 0) {
        struct tm* modified_time = localtime(&buf.st_mtime);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", modified_time);
        return std::string(buffer);
    }
    return "";
#endif
}

std::string FileTool::ParentDir(char* path) {
    int len = strlen(path) - 1;
    for (int i = len; i > 0; i--) {
        if (path[i] == '/' || path[i] == '\\') {
            path[i + 1] = 0;
            break;
        }
    }
    std::string dir(path);
    return dir;
}

std::string FileTool::ParentDir(const std::string& path) {
    size_t len = path.length();
    for (size_t i = len; i > 0; i--) {
        if (path[i] == '/' || path[i] == '\\') {
            return path.substr(0, i);
        }
    }
    return "";
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

std::vector<std::string> FileTool::LoadDir(const std::string& /*path*/, const std::string& /*file_suffix*/) {
    std::vector<std::string> result;
    return result;
}

bool FileTool::FileRegexReplace(const std::string& file_path, const std::regex& rule, const std::string& replace) {
    std::string content = LoadFile(file_path);
    if (content == "") {
        return false;
    }
    content = std::regex_replace(content, rule, replace);
    return SaveFile(file_path, content);
}

char FileTool::GetPathSeparator() {
#if defined(_WIN32) || defined(_WIN64)
    return '\\';
#else
    return '/';
#endif
}

std::string FileTool::RemoveFileSuffix(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        return filename.substr(0, pos);
    }
    return filename;
}

std::string FileTool::GetFileSuffix(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        return filename.substr(pos + 1);
    }
    return "";
}

// 保存文件
bool FileTool::SaveFile(const std::string& file_path, const std::string& content) {
    // 检查文件是否存在，如果不存在，就递归创建之
    if (!IsFileExists(file_path)) {
        std::string parent_dir = ParentDir(file_path);
        MakeDirs(parent_dir);
    }
    std::ofstream out(file_path);
    out << content;
    out.close();
    return true;
}

bool FileTool::DeleteAllFiles(const std::string& dir_path) {
#if defined(_WIN32) || defined(_WIN64)
    intptr_t hFile = 0;
    struct _finddata_t fileinfo;
    std::string path;
    if ((hFile = _findfirst(path.assign(dir_path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            if ((fileinfo.attrib & _A_SUBDIR)) {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
                    DeleteAllFiles(path.assign(dir_path).append("\\").append(fileinfo.name));
                }
            } else {
                path.assign(dir_path).append("\\").append(fileinfo.name);
                std::remove(path.c_str());  // 删除文件
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
        return true;
    }
    return false;
#else
    DIR* folder = opendir(dir_path.c_str());
    struct dirent* file = new dirent();
    std::string path;
    while ((file = readdir(folder)) != NULL) {
        if (strncmp(file->d_name, ".", 1) == 0 || strncmp(file->d_name, "..", 2) == 0) {
            continue;
        }
        if (file->d_type == 8) {  // 文件
            path.assign(dir_path).append("/").append(file->d_name);
            remove(path.c_str());        // 删除文件
        } else if (file->d_type == 4) {  // 文件夹
            path.assign(dir_path).append("/").append(file->d_name);
            DeleteAllFiles(path);
        }
    }
    if (folder) {
        closedir(folder);
        return true;
    }
    return false;
#endif
}

// 递归创建文件夹
bool FileTool::MakeDirs(const std::string& path) {
#if defined(_WIN32) || defined(_WIN64)
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
#if defined(_WIN32) || defined(_WIN64)
                pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
                return false;
            if (!MakeDirs(path.substr(0, pos)))
                return false;
        }
#if defined(_WIN32) || defined(_WIN64)
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

std::string FileTool::MultiRegexReplace(const std::string& text,
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

// 跨平台读取文本行，解决 std::getline Linux环境下无法正确读取\r\n结尾的文件
std::ifstream& FileTool::GetLine(std::ifstream& ifs, std::string& line) {
    std::string _line;
    if (std::getline(ifs, _line)) {
        if (_line.size() && _line[_line.size() - 1] == '\r') {
            line = _line.substr(0, _line.size() - 1);
        } else {
            line = _line;
        }
    }
    return ifs;
}

bool FileTool::GetLastLineOfFile(const std::string& filename, std::string& line) {
    // std::ifstream file(filename);
    // if (!file.is_open()) {
    //     return "";
    // }

    // std::string line, lastLine;
    // while (std::getline(file, line)) {
    //     lastLine = line;
    // }

    // file.close();
    // return lastLine;
    std::ifstream ifs;
    char buffer[512];  // 一行最大512个字符
    ifs.open(filename);
    if (ifs.is_open()) {
        ifs.seekg(-512, std::ios_base::end);  // 定位到文件末尾往前 1024 个字节
        ifs.read(buffer, 512);
        int start_pos = 0;
        int end_pos = ifs.gcount() - 1;
        while (buffer[end_pos] == '\n' || buffer[end_pos] == '\r') {  // 过滤末尾的\n或者\r
            end_pos--;
        }
        for (int i = end_pos; i >= 0; i--) {  // 遍历实际读取的字节数
            if (buffer[i] == '\n') {
                start_pos = i + 1;
                break;
            }
        }
        line = "";
        for (int i = start_pos; i <= end_pos; i++) {
            line += buffer[i];
        }
        ifs.close();
        return true;
    }
    return false;
}

// 删除文件最后一行,解决日K线文件增量保存的问题
bool FileTool::RemoveLastLineOfFile(const std::string& filename) {
    std::fstream inputFile(filename, std::ios::binary | std::ios::out | std::ios::in);
    if (!inputFile) {
        return false;
    }

    inputFile.seekg(0, std::ios::end);
    std::streamoff endPos = inputFile.tellg();

    bool foundNewLine = false;
    char ch;
    std::streamoff currentPos = endPos;
    size_t line = 0;
    while (currentPos > 0) {  // 忽略最后一行
        inputFile.seekg(--currentPos);
        inputFile.get(ch);
        if (ch == '\n' || (ch == '\r' && currentPos > 0 && inputFile.peek() == '\n')) {
            line += 1;
        } else {
            break;
        }
    }

    while (currentPos > 0) {
        inputFile.seekg(--currentPos);
        inputFile.get(ch);
        if (ch == '\n' || (ch == '\r' && currentPos > 0 && inputFile.peek() == '\n')) {
            foundNewLine = true;
            break;
        }
    }

    if (!foundNewLine) {  // 没有找到换行符，说明文件只有一行或者没有内容，不做处理
        inputFile.close();
        if (line == 1) {
            TruncateFileSize(filename, 0);
        }
        return true;
    }

    inputFile.close();
    TruncateFileSize(filename, currentPos);
    return true;
}

bool FileTool::TruncateFileSize(const std::string& filename, size_t size) {
#if defined(_WIN32) || defined(_WIN64)
    LARGE_INTEGER large_size;
    large_size.QuadPart = size;
    HANDLE hFile = CreateFile(filename.c_str(), FILE_SHARE_READ | FILE_SHARE_WRITE, 0, NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    SetFilePointerEx(hFile, large_size, NULL, FILE_BEGIN);
    if (SetEndOfFile(hFile) == 0) {
        std::cerr << GetLastError();
        if (hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
        }
        return false;
    }
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
    }
    return true;
#else
    int fd = open(filename.c_str(), O_RDWR);
    if (fd == -1) {
        return false;
    }
    if (ftruncate(fd, size) == -1) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
#endif
}
