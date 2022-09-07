#pragma once

#include "utils/Filesystem.hpp"
#include "utils/storage/AbstractStorageFactory.hpp"
#include "utils/storage/FileStorage.hpp"

namespace feather::utils::storage
{
    class FileStorageFactory : public AbstractStorageFactory
    {
    private:
        std::unique_ptr<utils::Filesystem> filesystem;

    public:
        FileStorageFactory();
        std::shared_ptr<AbstractStorage> getStorage(std::string const &, utils::datatypes::Uuid const &) override;
        std::shared_ptr<AbstractStorage> getStorageWithUtf8Check(std::string const &) override;
        bool isFileExist(std::string const &) override;
        void changeFileName(std::string, std::string, utils::datatypes::Uuid const &) override;
        void removeAllStorages() override;
        void removeStorage(std::string const &) override;
        void removeSuffixFromStorageName(std::string const &&, std::string const &) override;
        ~FileStorageFactory();
    };
} // namespace feather::utils::storage
