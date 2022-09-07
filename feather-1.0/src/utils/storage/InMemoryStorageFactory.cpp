#include "utils/storage/InMemoryStorageFactory.hpp"

#include <iostream>

namespace feather::utils::storage
{
    InMemoryStorageFactory::InMemoryStorageFactory()
    {
    }

    std::set<std::u32string> InMemoryStorageFactory::getAllFiles()
    {
        std::set<std::u32string> files;
        for (auto const &e : storageSpace)
        {
            std::string eFileName = e.first.substr(e.first.find_last_of('/') + 1UL);
            std::u32string s;
            std::copy(eFileName.begin(), eFileName.end(), std::back_inserter(s));
            files.insert(s);
        }
        return files;
    }

    std::shared_ptr<AbstractStorage> InMemoryStorageFactory::getStorage(std::string const &name, utils::datatypes::Uuid const &windowUUID)
    {
        if (storageSpace.count(name))
        {
            return std::make_shared<InMemoryStorage>(name, storageSpace[name]->getRawStream(), storageSpace[name]->getLastModificationTimestamp());
        }
        return storageSpace.insert({name, std::make_shared<InMemoryStorage>(name)}).first->second;
    }

    std::shared_ptr<AbstractStorage> InMemoryStorageFactory::getStorageWithUtf8Check(std::string const &name)
    {
        if (storageSpace.count(name))
        {
            return std::make_shared<InMemoryStorage>(name, storageSpace[name]->getRawStream(), storageSpace[name]->getLastModificationTimestamp());
        }
        return storageSpace.insert({name, std::make_shared<InMemoryStorage>(name)}).first->second;
    }

    void InMemoryStorageFactory::changeFileName(std::string from, std::string to, utils::datatypes::Uuid const &windowUUID)
    {
        auto storage = storageSpace[from];
        storageSpace.erase(from);
        storageSpace.erase(to);
        storageSpace.insert({to, storage});
        storage->changeName(to);
    }

    void InMemoryStorageFactory::removeAllStorages()
    {
        storageSpace.clear();
    }

    bool InMemoryStorageFactory::isFileExist(std::string const &fileName)
    {
        return storageSpace.find(fileName) != storageSpace.end();
    }

    void InMemoryStorageFactory::removeStorage(std::string const &name)
    {
        storageSpace.erase(name);
    }

    void InMemoryStorageFactory::removeSuffixFromStorageName(std::string const &&name, std::string const &suffix)
    {
        auto storage = storageSpace.at(name);
        storageSpace.erase(name);
        storage->changeName(name.substr(0, name.find(suffix)));
        storageSpace.emplace(storage->getName(), storage);
    }

    InMemoryStorageFactory::~InMemoryStorageFactory() = default;

    std::unordered_map<std::string, std::shared_ptr<AbstractStorage>> InMemoryStorageFactory::storageSpace;
} // namespace feather::utils::storage
