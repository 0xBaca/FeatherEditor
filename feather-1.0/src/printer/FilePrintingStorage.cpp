#include "printer/FilePrintingStorage.hpp"
#include "utils/Serializer.hpp"
#include "utils/Utf8Util.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/storage/FileStorageFactory.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::printer
{
    FilePrintingStorage::FilePrintingStorage(std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactoryArg)
        : storageFactory(storageFactoryArg)
    {
    }

    void FilePrintingStorage::addToNewChanges(utils::datatypes::Uuid const &windowUUID, pair pos, std::vector<char> const &buffer, size_t bufferSize)
    {
        std::vector<char> serializedMetadata;
        if (dynamic_cast<utils::storage::FileStorageFactory *>(storageFactory.get()))
        {
            storageFactory->removeStorage(getChangesPath(windowUUID, pos.first));
        }
        auto existingFile = getChangesFile(pos.first, getChangesPath(windowUUID, pos.first), windowUUID);
        auto newFile = getChangesFile(pos.first, getChangesPath(windowUUID, pos.first, utils::datatypes::Strings::TEMP_FILE_SUFFIX), windowUUID);

        size_t existingSize = 0;
        std::stringstream stringified;
        if (!existingFile.first)
        {
            stringified.str(existingFile.second->getLine());
            stringified >> existingSize;
        }
        std::string serializedSize = std::to_string(existingSize + bufferSize) + '\n';
        std::copy(serializedSize.cbegin(), serializedSize.cend(), std::back_inserter(serializedMetadata));
        newFile.second->writeChunk(serializedMetadata, serializedMetadata.size());
        std::vector<char> oldFileBuffer(configuration->getMemoryBytesRelaxed(), 0);

        for (size_t currFilePos = 0; currFilePos <= existingSize;)
        {
            size_t canRead = (currFilePos < pos.second) ? std::min(pos.second - currFilePos, configuration->getMemoryBytesRelaxed()) : std::min(existingSize - currFilePos, configuration->getMemoryBytesRelaxed());
            if (currFilePos == pos.second)
            {
                newFile.second->writeChunk(buffer, bufferSize);
            }

            if (!canRead)
            {
                break;
            }
            else
            {
                if (canRead < oldFileBuffer.size())
                {
                    oldFileBuffer.resize(canRead);
                }
                existingFile.second->readChunk(oldFileBuffer, oldFileBuffer.size(), false);
                newFile.second->writeChunk(oldFileBuffer, oldFileBuffer.size());
                currFilePos += oldFileBuffer.size();
            }
        }
        storageFactory->removeStorage(existingFile.second->getName());
        storageFactory->removeSuffixFromStorageName(newFile.second->getName(), utils::datatypes::Strings::TEMP_FILE_SUFFIX);
    }

    std::pair<bool, std::shared_ptr<utils::storage::AbstractStorage>> FilePrintingStorage::getChangesFile(size_t pos, std::string &&name, utils::datatypes::Uuid const &windowUUID)
    {
        std::vector<char> watermark(FEATHER_WATERMARK.begin(), FEATHER_WATERMARK.end());
        std::shared_ptr<utils::storage::AbstractStorage> file = storageFactory->getStorage(std::forward<std::string>(name), windowUUID);
        if (0 == file->getSize())
        {
            std::string posAsString(std::to_string(pos));
            std::vector<char> posChunk(posAsString.cbegin(), posAsString.cend());
            posChunk.push_back('\n');
            file->writeChunk(watermark, watermark.size());
            file->writeChunk(posChunk, posChunk.size());
            return std::make_pair(true, file);
        }
        // Check is feather watermark exists
        if (!utils::helpers::Lambda::isValidFeatherStorage(file))
        {
            throw utils::exception::FeatherRuntimeException("Storage directory corrupted...");
        }
        file->getLine();
        file->getLine();
        return std::make_pair(false, file);
    }

    std::shared_ptr<utils::storage::AbstractStorage> FilePrintingStorage::getStorage(std::string &&name, utils::datatypes::Uuid const &windowUUID)
    {
        return storageFactory->getStorage(std::forward<std::string>(name), windowUUID);
    }

    std::string FilePrintingStorage::getChangesPath(utils::datatypes::Uuid const &windowUUID, size_t pos, std::string const suffix)
    {
        return configuration->getStorageDirectory() + windowUUID.getString() + "/" + std::to_string(pos) + utils::datatypes::Strings::STORAGE_EXTENSION + suffix;
    }

    std::string FilePrintingStorage::getDeletionsPath(utils::datatypes::Uuid const &windowUUID, std::string const &fileName)
    {
        return configuration->getStorageDirectory() + windowUUID.getString() + "/" + fileName;
    }

    std::vector<char> FilePrintingStorage::getNewChanges(utils::datatypes::Uuid const &windowUUID, size_t pos, size_t offset, size_t bytesToRead)
    {
        auto fileWithChanges = getChangesFile(pos, getChangesPath(windowUUID, pos), windowUUID);
        size_t totalChangesBytes = 0;
        std::stringstream stringified;
        stringified.str(fileWithChanges.second->getLine());
        stringified >> totalChangesBytes;
        if (totalChangesBytes <= offset)
        {
            return std::vector<char>{};
        }
        fileWithChanges.second->setCursor(fileWithChanges.second->getCurrentPointerPosition() + offset);
        bytesToRead = std::min(totalChangesBytes - offset, bytesToRead);
        std::vector<char> data(bytesToRead, 0);
        fileWithChanges.second->readChunk(data, bytesToRead, false);
        return data;
    }

    void FilePrintingStorage::removeChanges(utils::datatypes::Uuid const &windowUUID, size_t pos)
    {
        storageFactory->removeStorage(getChangesFile(pos, getChangesPath(windowUUID, pos), windowUUID).second->getName());
    }

    void FilePrintingStorage::dumpChanges(utils::datatypes::Uuid const &windowUUID, pair pos, std::vector<char> const &buffer)
    {
        if (dynamic_cast<utils::storage::FileStorageFactory *>(storageFactory.get()))
        {
            storageFactory->removeStorage(getChangesPath(windowUUID, pos.first));
        }
        auto file = getStorage(getChangesPath(windowUUID, pos.first), windowUUID);
        utils::Serializer::serialize(pos.first, buffer, file);
    }

    void FilePrintingStorage::dumpColors(utils::datatypes::Uuid const &windowUUID, std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &textChunksColors, std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &textColors, size_t colorPriority)
    {
        auto file = getStorage(getDeletionsPath(windowUUID, utils::datatypes::Strings::COLORS_FILE_NAME), windowUUID);
        utils::Serializer::serialize(textChunksColors, textColors, colorPriority, file);
    }

    void FilePrintingStorage::dumpDeletions(utils::datatypes::Uuid const &windowUUID, std::map<pair, pair> const &deletions)
    {
        auto file = getStorage(getDeletionsPath(windowUUID, utils::datatypes::Strings::DELETIONS_FILE_NAME), windowUUID);
        utils::Serializer::serialize(deletions, file);
    }

} // namespace feather::printer
