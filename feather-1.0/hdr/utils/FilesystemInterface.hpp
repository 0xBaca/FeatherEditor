#pragma once

#include <string>
#include <vector>

namespace feather::utils
{
    class FilesystemInterface
    {
    public:
        virtual std::vector<std::string> getAllFiles(std::string) = 0;
        virtual std::string getAbsolutePath(std::string const &) = 0;
        virtual size_t getFileSize(std::string) = 0;
        virtual std::string getCurrentDirectory() = 0 ;
        virtual std::string getUserName() = 0;
        virtual bool hasFolderExecutePermissions(std::string) = 0;
        virtual bool hasFolderWritePermissions(std::string) = 0;
        virtual bool hasDiskEnoughSpace(std::string, size_t) = 0;
        virtual bool isRegularFile(std::string) = 0;
        virtual bool isFileDirectory(std::string) = 0;
        virtual bool isFileExist(std::string) = 0;
        virtual bool isFolderEmpty(std::string) = 0;
        virtual bool isFolderExist(std::string) = 0;
        virtual void createDirectory(std::string const &, std::string const &) = 0;
        virtual void removeDir(std::string) = 0;
        virtual void removeFile(std::string) = 0;
        virtual ~FilesystemInterface() = 0;
    };
} // namespace feather::utils
