#include <wx/dir.h>
#include <wx/xml/xml.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include "util/FileTool.h"
#include "util/Global.h"

#ifdef _WIN32
#define BUILD_ROOT "E:\\work\\iwealth\\build\\code_blocks_x86_64"
#define SMITH "\\wxsmith"
#define ORIGIN_SMITH_ROOT BUILD_ROOT SMITH
#define UI_INCLUDE "E:\\work\\iwealth\\include\\ui\\components"
#define UI_SRC "E:\\work\\iwealth\\src\\ui\\components"
#define TARGET_SMITH_ROOT "E:\\work\\iwealth\\src\\ui\\wxsmith"
#define PROJECT_FILE BUILD_ROOT "\\iwealth.cbp"
#else
#define BUILD_ROOT "/home/greatwall/work/iwealth/build/code_blocks_aarch64"
#define SMITH "/wxsmith"
#define ORIGIN_SMITH_ROOT BUILD_ROOT SMITH
#define UI_INCLUDE "/home/greatwall/work/iwealth/include/ui/components"
#define UI_SRC "/home/greatwall/work/iwealth/src/ui/components"
#define TARGET_SMITH_ROOT "/home/greatwall/work/iwealth/src/ui/wxsmith"
#define PROJECT_FILE BUILD_ROOT "/iwealth.cbp"
#endif

void copyFile(const std::string& src_path, const std::string& dest_path) {
    std::ifstream src(src_path, std::ios::binary);
    std::ofstream dst(dest_path, std::ios::binary);
    dst << src.rdbuf();
}

// 过滤指定后缀的目录
std::vector<wxString> getFilterFiles(const wxString& dir_path, const std::string& file_suffix) {
    std::vector<std::string> suffix_list = split(file_suffix, ',');
    std::unordered_map<std::string, bool> suffix_map;
    for (const std::string& suffix : suffix_list) {
        suffix_map.insert({suffix, true});
    }

    wxDir dir(dir_path);
    std::vector<wxString> files;
    if (!dir.IsOpened()) {
        return files;
    }
    wxString filename;
    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    while (cont) {
        std::string file_suffix = FileTool::GetFileSuffix(filename.ToStdString());
        if (suffix_map.find(file_suffix) != suffix_map.end()) {
            files.push_back(filename);
        }
        cont = dir.GetNext(&filename);
    }
    return files;
}

std::vector<wxString> getFiles(const wxString& dir_path) {
    wxDir dir(dir_path);
    std::vector<wxString> files;
    if (!dir.IsOpened()) {
        return files;
    }
    wxString filename;
    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    while (cont) {
        files.push_back(filename);
        cont = dir.GetNext(&filename);
    }
    return files;
}

// 拷贝smith目录下的所有文件
void copySmithFiles() {
    std::vector<wxString> smith_files = getFiles(ORIGIN_SMITH_ROOT);
    for (const wxString& filename : smith_files) {
        std::string origin = std::string(ORIGIN_SMITH_ROOT) + FileTool::GetPathSeparator() + filename.ToStdString();
        std::string target = std::string(TARGET_SMITH_ROOT) + FileTool::GetPathSeparator() + filename.ToStdString();
        std::cout << "copy from " << origin << " to " << target << std::endl;
        wxCopyFile(origin, target);
    }
}

// 拷贝build目录下的所有.h/.cpp后缀文件
void copySourceFiles(std::vector<wxString>& generated_source_files) {
    std::string origin = "";
    std::string target = "";
    for (const wxString& filename : generated_source_files) {
        std::string file_suffix = FileTool::GetFileSuffix(filename.ToStdString());
        if (file_suffix == "h") {
            origin = std::string(BUILD_ROOT) + FileTool::GetPathSeparator() + filename.ToStdString();
            target = std::string(UI_INCLUDE) + FileTool::GetPathSeparator() + filename.ToStdString();
        } else {
            origin = std::string(BUILD_ROOT) + FileTool::GetPathSeparator() + filename.ToStdString();
            target = std::string(UI_SRC) + FileTool::GetPathSeparator() + filename.ToStdString();
        }
        wxCopyFile(origin, target);
        if (file_suffix == "cpp") {
            std::string header = FileTool::RemoveFileSuffix(filename.ToStdString()) + ".h";
            std::regex rule("#include \"(" + header + ")\"");
            std::string replace = "#include \"ui/components/$1\"";
            FileTool::FileRegexReplace(target, rule, replace);
        }
        std::cout << "copy from " << origin << " to " << target << std::endl;
    }
}

// 替换cbp文件中的路径
void updateCodeBlockProjectFile(std::vector<wxString>& generated_source_files) {
    std::string cbp_content = FileTool::LoadFile(std::string(PROJECT_FILE));
    for (const wxString& source_file : generated_source_files) {
        std::string file_suffix = FileTool::GetFileSuffix(source_file.ToStdString());
        if (file_suffix == "h") {
            std::regex rule1("hdr=\"(" + source_file + ")\"");
            cbp_content = std::regex_replace(cbp_content, rule1, "hdr=\"../../include/ui/$1\"");
            std::regex rule2("<Unit filename=\"(" + source_file + ")\" />");
            cbp_content =
                std::regex_replace(cbp_content, rule2, "<Unit filename=\"../../include/ui/components/$1\" />");
        } else {
            std::regex rule3("<Unit filename=\"(" + source_file + ")\" />");
            cbp_content = std::regex_replace(cbp_content, rule3, "<Unit filename=\"../../src/ui/components/$1\" />");

            std::string base_name = FileTool::RemoveFileSuffix(source_file.ToStdString());
            std::regex rule4("wxs=\"wxsmith/(" + base_name + ").wxs");
            cbp_content = std::regex_replace(cbp_content, rule4, "wxs=\"../../src/ui/wxsmith/$1.wxs");

            std::regex rule5("src=\"(" + source_file + ")\"");
            cbp_content = std::regex_replace(cbp_content, rule5, "src=\"../../src/ui/$1\"");

            std::regex rule6("<Unit filename=\"wxsmith/(" + base_name + ").wxs\" />");
            cbp_content = std::regex_replace(cbp_content, rule6, "<Unit filename=\"../../src/ui/wxsmith/$1.wxs\" />");
        }
    }
    std::cout << "++++++++++++++  CodeBlocks::ProjectFile  +++++++++++++++" << std::endl;
    std::cout << cbp_content << std::endl;
    if (generated_source_files.size() > 0) {
        FileTool::SaveFile(std::string(PROJECT_FILE), cbp_content);
    }
}
// 删除wxSmith生成的老的文件
void deleteOldSmithFiles(std::vector<wxString>& generated_source_files) {
    std::string smith_dir = std::string(ORIGIN_SMITH_ROOT);
    FileTool::DeleteAllFiles(smith_dir);
    std::string origin = "";
    for (const wxString& source_file : generated_source_files) {
        origin = std::string(BUILD_ROOT) + FileTool::GetPathSeparator() + source_file.ToStdString();
        std::remove(origin.c_str());
    }
}

void fixSmithPluginGeneratedFiles() {
    copySmithFiles();
    std::vector<wxString> generated_source_files = getFilterFiles(BUILD_ROOT, "h,cpp");
    copySourceFiles(generated_source_files);
    updateCodeBlockProjectFile(generated_source_files);
    deleteOldSmithFiles(generated_source_files);
}

int main(int argc, char* argv[]) {
    fixSmithPluginGeneratedFiles();
    return 0;
}