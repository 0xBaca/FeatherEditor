#include "config/Config.hpp"
#include "utils/exception/FeatherRuntimeException.hpp"
#include "utils/storage/FileStorageFactory.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::utils::storage
{
    FileStorageFactory::FileStorageFactory() : filesystem(new Filesystem())
    {
    }

    std::shared_ptr<AbstractStorage> FileStorageFactory::getStorage(std::string const &name, utils::datatypes::Uuid const &windowUUID)
    {
#ifndef _FEATHER_TEST_
        if (!filesystem->isFileExist(configuration->getStorageDirectory() + windowUUID.getString()))
        {
            filesystem->createDirectory(configuration->getStorageDirectory(), windowUUID.getString());
        }
#endif
        return std::make_shared<FileStorage>(name);
    }

    std::shared_ptr<AbstractStorage> FileStorageFactory::getStorageWithUtf8Check(std::string const &name)
    {
        return std::make_shared<FileStorage>(name, true);
    }

    void FileStorageFactory::changeFileName(std::string from, std::string to, utils::datatypes::Uuid const &windowUUID)
    {
        removeStorage(to);
        getStorage(from, windowUUID)->changeName(to);
    }

    bool FileStorageFactory::isFileExist(std::string const &fileName)
    {
        return filesystem->isFileExist(fileName);
    }

    void FileStorageFactory::removeAllStorages()
    {
        //TODO
    }

    void FileStorageFactory::removeStorage(std::string const &name)
    {
        if (filesystem->isFileExist(name) && ::remove(name.c_str()))
        {
            throw utils::exception::FeatherRuntimeException("Could not delete file : ", name);
        }
    }

    void FileStorageFactory::removeSuffixFromStorageName(std::string const &&name, std::string const &suffix)
    {
        std::string newName = name.substr(0, name.find(suffix));
        if (0 != rename(name.c_str(), newName.c_str()))
        {
            throw utils::exception::FeatherRuntimeException("Could not change file name from ", name, " to ", newName);
        }
    }

    FileStorageFactory::~FileStorageFactory() {}
} // namespace feather::utils::storage
