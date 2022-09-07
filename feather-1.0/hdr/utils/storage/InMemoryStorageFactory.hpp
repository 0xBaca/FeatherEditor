#pragma once

#include "utils/storage/AbstractStorageFactory.hpp"
#include "utils/storage/InMemoryStorage.hpp"

#include <set>
#include <unordered_map>

namespace feather::utils::storage
{
    class InMemoryStorageFactory : public AbstractStorageFactory
    {
    private:
        static std::unordered_map<std::string, std::shared_ptr<AbstractStorage>> storageSpace;

    public:
        static std::set<std::u32string> getAllFiles();
        InMemoryStorageFactory();
        std::shared_ptr<AbstractStorage> getStorage(std::string const &, utils::datatypes::Uuid const &) override;
        std::shared_ptr<AbstractStorage> getStorageWithUtf8Check(std::string const &) override;
        bool isFileExist(std::string const &) override;
        void changeFileName(std::string, std::string, utils::datatypes::Uuid const &) override;
        void removeAllStorages() override;
        void removeStorage(std::string const &) override;
        void removeSuffixFromStorageName(std::string const &&, std::string const &) override;
        ~InMemoryStorageFactory();
    };
} // namespace feather::utils::storage