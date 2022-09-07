#pragma once

#include "utils/storage/AbstractStorage.hpp"

#include <memory>

#include "utils/datatypes/Uuid.hpp"

namespace feather::utils::storage
{
    class AbstractStorageFactory
    {
    public:
        virtual std::shared_ptr<AbstractStorage> getStorage(std::string const &, utils::datatypes::Uuid const &) = 0;
        virtual std::shared_ptr<AbstractStorage> getStorageWithUtf8Check(std::string const &) = 0;
        virtual bool isFileExist(std::string const &) = 0;
        virtual void changeFileName(std::string, std::string, utils::datatypes::Uuid const &) = 0;
        virtual void removeAllStorages() = 0;
        virtual void removeStorage(std::string const &) = 0;
        virtual void removeSuffixFromStorageName(std::string const &&, std::string const &) = 0;
        virtual ~AbstractStorageFactory() = 0;
    };
} // namespace feather::utils::storage
