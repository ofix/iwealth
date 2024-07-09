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
    static std::string GetFileModifiedTime(const std::string& path);
    static bool MakeDirs(const std::string& path);
    static std::vector<std::string> LoadDir(const std::string& path, const std::string& file_suffix);
    static char GetPathSeparator();
    static bool FileRegexReplace(const std::string& file_path, const std::regex& rule, const std::string& replace);
    static std::string RemoveFileSuffix(const std::string& filename);
    static bool DeleteAllFiles(const std::string& dir_path);
    static std::string GetFileSuffix(const std::string& filename);
    static std::string LoadFile(const std::string& filePath);
    static bool SaveFile(const std::string& filePath, const std::string& content);
    static std::regex BuildRegexFromMap(const std::map<std::string, std::string>& map);
    static std::string MultiRegexReplace(const std::string& text,
                                         const std::map<std::string, std::string>& replacement_map);
    static std::ifstream& GetLine(std::ifstream& ifs, std::string& line);
};

#endif  // FILETOOL_H
