#include "utils/FilesystemInterface.hpp"

#include <gmock/gmock.h>

using namespace feather::utils;

namespace testing::mocks
{
    class FilesystemMock : public FilesystemInterface
    {
    public:
        MOCK_METHOD0(getCurrentDirectory, std::string());
        MOCK_METHOD0(getUserName, std::string());
        MOCK_METHOD1(getAbsolutePath, std::string(std::string const &));
        MOCK_METHOD1(getAllFiles, std::vector<std::string>(std::string));
        MOCK_METHOD1(getFileSize, size_t(std::string));
        MOCK_METHOD1(hasFolderExecutePermissions, bool(std::string));
        MOCK_METHOD1(hasFolderWritePermissions, bool(std::string));
        MOCK_METHOD1(isFileDirectory, bool(std::string));
        MOCK_METHOD1(isFileExist, bool(std::string));
        MOCK_METHOD1(isFolderEmpty, bool(std::string));
        MOCK_METHOD1(isFolderExist, bool(std::string));
        MOCK_METHOD1(isRegularFile, bool(std::string));
        MOCK_METHOD1(removeDir, void(std::string));
        MOCK_METHOD1(removeFile, void(std::string));
        MOCK_METHOD2(createDirectory, void(std::string const &, std::string const &));
        MOCK_METHOD2(hasDiskEnoughSpace, bool(std::string, size_t));
    };
} // namespace testing::mocks
