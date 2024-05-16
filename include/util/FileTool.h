#ifndef FILETOOL_H
#define FILETOOL_H

#include <fstream>
#include <regex>
#include <string>
#include <vector>

class FileTool {
   public:
    static bool IsFileExists(const std::string& path);
    static std::string CurrentPath();
    static std::string ParentDir(char* path);
    static std::string ParentDir(const std::string& path);
    static bool MakeDirs(const std::string& path);
    static std::vector<std::string> LoadDir(const std::string& path,
                                            const std::string& file_suffix);
    static std::string LoadFile(const std::string& filePath);
    static bool SaveFile(const std::string& filePath, const std::string& content);
    static std::regex BuildRegexFromMap(const std::map<std::string, std::string>& map);
    static std::string MultiRegexReplace(
        const std::string& text,
        const std::map<std::string, std::string>& replacement_map);
};

#endif  // FILETOOL_H
