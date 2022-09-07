#include "config/Config.hpp"
#include "utils/Filesystem.hpp"
#include "utils/exception/FeatherRuntimeException.hpp"

#include <filesystem>

#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::utils
{
    std::vector<std::string> Filesystem::getAllFiles(std::string name)
    {
        std::vector<std::string> files;
        DIR *dirp = opendir(name.c_str());
        struct dirent *dp;
        while ((dp = readdir(dirp)) != NULL)
        {
            files.push_back(dp->d_name);
        }
        closedir(dirp);
        return files;
    }

    std::string Filesystem::getAbsolutePath(std::string const &path)
    {
        std::string absolutePath;
        if (path.empty())
        {
            return path;
        }
        if ('~' == path.front())
        {
            absolutePath = configuration->getHomeDirectory() + path.substr(1UL);
        }
        else
        {
            absolutePath = path;
        }
        char resolved_path[PATH_MAX];
        if (realpath(absolutePath.c_str(), resolved_path) != NULL)
        {
            return resolved_path;
        }
        throw utils::exception::FeatherRuntimeException("Path is invalid :", absolutePath);
    }

    size_t Filesystem::getFileSize(std::string fileName)
    {
        return std::filesystem::file_size(fileName);
    }

    std::string Filesystem::getCurrentDirectory()
    {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            return std::string(cwd);
        }
        throw utils::exception::FeatherRuntimeException("Could get current dir");
    }

    std::string Filesystem::getUserName()
    {
        uid_t uid = geteuid();
        struct passwd *pw = getpwuid(uid);
        if (pw)
        {
            return std::string(pw->pw_name);
        }
        throw utils::exception::FeatherRuntimeException("");
    }

    bool Filesystem::hasFolderExecutePermissions(std::string fileName)
    {
        //std::string folderName = fileName.substr(0, fileName.find_last_of('/') + 1);
        return !access(fileName.c_str(), X_OK);
    }

    bool Filesystem::hasFolderWritePermissions(std::string fileName)
    {
        std::string folderName = fileName.substr(0, fileName.find_last_of('/'));
        return !access(folderName.c_str(), W_OK);
    }

    bool Filesystem::hasDiskEnoughSpace(std::string fileName, size_t required)
    {
        try
        {
            std::string dir = fileName.substr(0, fileName.find_first_of('/', 1));
            return std::filesystem::space(dir).available > required;
        }
        catch (std::exception const &e)
        {
            return false;
        }
    }

    bool Filesystem::isRegularFile(std::string fileName)
    {
        struct stat path_stat;
        if (-1  == ::stat(fileName.c_str(), &path_stat))
        {
            return false;
        }
        return S_ISREG(path_stat.st_mode);
    }

    bool Filesystem::isFileDirectory(std::string fileName)
    {
        struct stat path_stat;
        if (-1 == ::stat(fileName.c_str(), &path_stat))
        {
            return false;
        }
        return S_ISDIR(path_stat.st_mode);
    }

    bool Filesystem::isFileExist(std::string fileName)
    {
        return access(fileName.c_str(), F_OK) != -1;
    }

    bool Filesystem::isFolderEmpty(std::string folderName)
    {
        return getAllFiles(folderName).size() == 2UL;
    }

    bool Filesystem::isFolderExist(std::string folderName)
    {
        return (-1 != ::access(folderName.c_str(), F_OK));
    }

    void Filesystem::createDirectory(std::string const &path, std::string const &dirName)
    {
        if (-1 == mkdir((getAbsolutePath(path) + "/" + dirName).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            throw utils::exception::FeatherRuntimeException("Cannot create dir :", path, dirName, errno);
        }
    }

    void Filesystem::removeDir(std::string dirName)
    {
        rmdir(dirName.c_str());
    }

    void Filesystem::removeFile(std::string fileName)
    {
        remove(fileName.c_str());
    }
} // namespace feather::utils
