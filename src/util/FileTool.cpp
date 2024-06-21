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
#endif
#include <cstdio>
#include <iostream>

bool FileTool::IsFileExists(const std::string& path) {
#ifdef _WIN32
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

// 获取文件修改时间，支持跨平台
std::string FileTool::GetFileModifiedTime(const std::string& path) {
#ifdef _WIN32
    HANDLE hFile =
        CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return "";
    }

    FILETIME modified_time;
    if (!GetFileTime(hFile, NULL, NULL, &modified_time)) {
        CloseHandle(hFile);
        return "";
    }

    SYSTEMTIME system_time;
    FileTimeToSystemTime(&modified_time, &system_time);

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
#ifdef _WIN32
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
#ifdef _WIN32
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
