#pragma once

#include <utils/FilesystemInterface.hpp>

namespace feather::utils
{
    class Filesystem : public FilesystemInterface
    {
    public:
        std::vector<std::string> getAllFiles(std::string) override;
        std::string getAbsolutePath(std::string const &) override;
        size_t getFileSize(std::string) override;
        std::string getCurrentDirectory() override;
        std::string getUserName() override;
        bool hasFolderExecutePermissions(std::string) override;
        bool hasFolderWritePermissions(std::string) override;
        bool hasDiskEnoughSpace(std::string, size_t) override;
        bool isRegularFile(std::string) override;
        bool isFileDirectory(std::string) override;
        bool isFileExist(std::string) override;
        bool isFolderEmpty(std::string) override;
        bool isFolderExist(std::string) override;
        void createDirectory(std::string const &, std::string const &) override;
        void removeDir(std::string) override;
        void removeFile(std::string) override;
    };
} // namespace feather::utils
