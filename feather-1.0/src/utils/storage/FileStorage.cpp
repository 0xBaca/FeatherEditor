#include "utils/Filesystem.hpp"
#include "utils/helpers/Lambda.hpp"
#include "utils/exception/FeatherInvalidArgumnetException.hpp"
#include "utils/exception/FeatherFileNotExistException.hpp"
#include "utils/exception/FeatherPermissionsException.hpp"
#include "utils/exception/FeatherRuntimeException.hpp"
#include "utils/storage/FileStorage.hpp"
#include "utils/Utf8Util.hpp"

#include <algorithm>
#include <codecvt>

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::utils::storage
{
    FileStorage::FileStorage(std::string const &fileNameArg, bool checkUtf8Compatibility) : fileName(fileNameArg), fileWasCreated(false)
    {
        if (!isFileExist(fileName))
        {
            //When we open some file, we always assume that it should exist
            std::ofstream createdFile(fileName, std::ios::binary);
            fileWasCreated = true;
        }
        fileName = filesystem->getAbsolutePath(fileNameArg);

        if (filesystem->isFileDirectory(fileName))
        {
            throw utils::exception::FeatherInvalidArgumentException("Could not read file :", fileName, "is a directory.");
        }
        else if (!filesystem->isRegularFile(fileName))
        {
            throw utils::exception::FeatherInvalidArgumentException("Could not read :", fileName, "is not a regular file.");
        }

        file = std::fstream(fileName, std::ios::binary | std::fstream::in | std::fstream::out);
        if (!file.good())
        {
            if (!filesystem->isFileExist(fileName))
            {
                throw utils::exception::FeatherFileNotExistException(file, "File :", fileName, "don't exist");
            }
            file = std::fstream(fileName, std::ios::binary | std::fstream::in);
            if (!file.good())
            {
                throw utils::exception::FeatherPermissionsException(file, "You don't have permissions to open :", fileName);
            }
        }

        if (stat(fileName.c_str(), &fileMetadata) == -1)
        {
            throw utils::exception::FeatherPermissionsException(file, "You don't have permissions to open :", fileName);
        }
        //Get file size
        file.seekg(0, file.end);
        fileSize = file.tellg();
        file.seekg(0, file.beg);
        lastModificationTimestamp = std::filesystem::last_write_time(fileName);
        lastReloadTimestamp = openTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

        if (!configuration->isHexMode())
        {
            try
            {
                file.imbue(std::locale(configuration->getLocale()));
            }
            catch (std::runtime_error const &e)
            {
                throw utils::exception::FeatherInvalidArgumentException("Error setting locale...");
            }
        }
    }

    inline bool FileStorage::isFileExist(std::string const &fileName)
    {
        return filesystem->isFileExist(fileName);
    }

    bool FileStorage::endOfStream()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        return getCurrentPointerPosition() == getSize();
    }

    bool FileStorage::isFirstLine()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        return !getCurrentPointerPosition();
    }

    bool FileStorage::isLastLine()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        size_t savedPosition = getCurrentPointerPosition();
        size_t noNewLinesMeet = 0UL;
        while (noNewLinesMeet != 2UL)
        {
            if (getSize() == getCurrentPointerPosition() || file.eof())
            {
                clearState();
                setCursor(savedPosition);
                return true;
            }
            else if (utils::helpers::Lambda::isNewLineChar(getCurrentCharacter()))
            {
                ++noNewLinesMeet;
            }
            getCurrentCharacterAndMoveCursor();
        }
        setCursor(savedPosition);
        return false;
    }

    bool FileStorage::isLastPosition(size_t pos)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        return (!pos && !getSize()) || (pos == getSize());
    }

    bool FileStorage::isBeginingOfCharacter(size_t pos)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        if (pos > getSize())
        {
            return false;
        }
        size_t currPos = getCurrentPointerPosition();
        setCursor(pos);
        bool result = Utf8Util::isBeginingOfCharacter(file.peek());
        setCursor(currPos);
        return result;
    }

    bool FileStorage::isValidFeatherStorage()
    {
        return getSize() >= sizeof(utils::datatypes::Metadata::MetadataPOD);
    }

    bool FileStorage::wasFileCreated() const
    {
        return fileWasCreated;
    }

    char FileStorage::peek()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        int peekedCharacter = file.peek();
        char toReturn = (-1 == peekedCharacter) ? 0 : peekedCharacter;
        file.clear();
        return toReturn;
    }

    char FileStorage::peek(size_t pos)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        size_t curPos = getCurrentPointerPosition();
        setCursor(pos);
        int peekedCharacter = file.peek();
        //Peeking 0 character means that -1 is returned, this is not what we wan't for binary reads
        char toReturn = (-1 == peekedCharacter) ? 0 : peekedCharacter;
        setCursor(curPos);
        file.clear();
        return toReturn;
    }

    char32_t FileStorage::getCharacterAtPos(size_t pos)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        size_t currPos = getCurrentPointerPosition();
        setCursor(pos);
        std::vector<char> chunk(4UL, 0);
        readChunk(chunk, std::min(4UL, getSize() - pos));

        unsigned short length = Utf8Util::getCharacterBytesTaken(chunk);
        chunk.resize(length);
        setCursor(currPos);
        return utils::helpers::Conversion::vectorOfCharToChar32_t(chunk);
    }

    char32_t FileStorage::getCurrentCharacter()
    {
#ifdef _DEBUG_MODE_

        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        size_t currPos = getCurrentPointerPosition();
        std::vector<char> chunk(4UL, 0);
        readChunk(chunk, std::min(4UL, getSize() - currPos));

        unsigned short length = Utf8Util::getCharacterBytesTaken(chunk);
        chunk.resize(length);
        setCursor(currPos);
        return utils::helpers::Conversion::vectorOfCharToChar32_t(chunk);
    }

    char32_t FileStorage::getCurrentCharacterAndMoveCursor()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        if (endOfStream())
        {
            return 0L;
        }
        size_t currPos = getCurrentPointerPosition();
        std::vector<char> chunk(4UL, 0);
        readChunk(chunk, std::min(4UL, getSize() - currPos));

        unsigned short length = Utf8Util::getCharacterBytesTaken(chunk);
        chunk.resize(length);
        setCursor(currPos + length);
        return utils::helpers::Conversion::vectorOfCharToChar32_t(chunk);
    }

    size_t FileStorage::getCurrentPointerPosition()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        if (!isFileExist(fileName))
        {
            throw utils::exception::FeatherFileNotExistException(file, "File :", fileName, "don't exist");
        }
        return static_cast<size_t>(file.tellg());
    }

    size_t FileStorage::alignToProperPosition(size_t position)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        if (getSize() <= position)
        {
            return getSize();
        }
        while (!isBeginingOfCharacter(position) && position)
        {
            --position;
        }
        return position;
    }

    size_t FileStorage::getPreviousCharacterPosition(size_t pos)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        size_t currPos = getCurrentPointerPosition();
        if (0UL == pos)
        {
            return pos;
        }
        setCursor(pos);
        size_t bytesMoved = moveCursorBackward();
        setCursor(currPos);
        return pos - bytesMoved;
    }

    size_t FileStorage::getPreviousCharacterPosition(size_t pos, size_t offset)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        while (offset--)
        {
            pos = getPreviousCharacterPosition(pos);
        }
        return pos;
    }

    size_t FileStorage::getBegining() const
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        return file.beg;
    }

    size_t FileStorage::getEnd() const
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        return file.end;
    }

    size_t FileStorage::getSize()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        return fileSize;
    }

    size_t FileStorage::moveCursorBackward()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        file.seekg(-1, file.cur);
        if (configuration->isHexMode())
        {
            return 1;
        }
        //MADI
        while (Utf8Util::isMiddleOfCharacter((char)file.peek()))
        {
            file.seekg(-1, file.cur);
        }
        size_t currPos = getCurrentPointerPosition();
        std::vector<char> chunk(4UL, 0);
        readChunk(chunk, std::min(4UL, getSize() - currPos));
        unsigned short bytesTaken = Utf8Util::getCharacterBytesTaken(chunk);
        setCursor(currPos);
        return bytesTaken;
    }

    size_t FileStorage::moveCursorToLineBegining(utils::Direction direction)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        if (utils::Direction::UP == direction && utils::helpers::Lambda::isNewLineChar(getCharacterAtPos(getCurrentPointerPosition())))
        {
            moveCursorBackward();
        }
        else if (utils::Direction::DOWN == direction && utils::helpers::Lambda::isNewLineChar(getCharacterAtPos(getCurrentPointerPosition())))
        {
            getCurrentCharacterAndMoveCursor();
        }

        while (getCurrentPointerPosition() && getCharacterAtPos(getCurrentPointerPosition()) != '\n')
        {
            moveCursorBackward();
        }
        if ('\n' == getCharacterAtPos(getCurrentPointerPosition()))
        {
            getCurrentCharacterAndMoveCursor();
        }
        return getCurrentPointerPosition();
    }

    void FileStorage::appendToBegining(std::vector<char> &data, std::shared_ptr<AbstractStorage> tmpStorage)
    {
        setCursor(0);
        tmpStorage->setCursor(0);
        tmpStorage->writeChunk(data, data.size());
        std::vector<char> oldFileBuffer(configuration->getMemoryBytesRelaxed(), 0);
        size_t oldCursorPos = getCurrentPointerPosition();
        for (size_t currFilePos = 0; currFilePos < getSize();)
        {
            size_t canRead = std::min(getSize() - currFilePos, configuration->getMemoryBytesRelaxed());
            if (canRead < oldFileBuffer.size())
            {
                oldFileBuffer.resize(canRead);
            }
            readChunk(oldFileBuffer, oldFileBuffer.size(), false);
            tmpStorage->setCursorAtTheEnd();
            tmpStorage->writeChunk(oldFileBuffer, oldFileBuffer.size());
            currFilePos += oldFileBuffer.size();
        }
        setCursor(oldCursorPos + data.size());
    }

    void FileStorage::copy(std::shared_ptr<utils::storage::AbstractStorage> from)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        size_t currPos = from->getCurrentPointerPosition(), bufferSize = 0;
        std::vector<char> buffer(configuration->getMemoryBytesRelaxed(), 0);
        while (currPos < from->getSize())
        {
            bufferSize = std::min(configuration->getMemoryBytesRelaxed(), from->getSize() - currPos);
            from->readChunk(buffer, bufferSize, false);
            writeChunk(buffer, bufferSize);
            currPos += bufferSize;
        }
    }

    size_t FileStorage::copy(std::shared_ptr<utils::storage::AbstractStorage> from, size_t bytesToCopy)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        std::vector<char> buffer(configuration->getMemoryBytesRelaxed() >= bytesToCopy ? bytesToCopy : configuration->getMemoryBytesRelaxed(), 0);
        from->readChunk(buffer, buffer.size());
        writeChunk(buffer, buffer.size());
        return buffer.size();
    }

    void FileStorage::copy(std::vector<char32_t> &from)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        for (char32_t v : from)
        {
            *this << v;
        }
    }

    void FileStorage::clearState()
    {
        file.clear();
    }

    void FileStorage::overrideMetadata(utils::datatypes::Metadata::MetadataPOD &&metadata)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        size_t oldCursor = getCurrentPointerPosition();
        setCursor(0);
        file.write((char *)&metadata, sizeof(utils::datatypes::Metadata::MetadataPOD));
        fileSize = (fileSize < sizeof(utils::datatypes::Metadata::MetadataPOD)) ? sizeof(utils::datatypes::Metadata::MetadataPOD) : fileSize;
        setCursor(oldCursor);
    }

    std::chrono::nanoseconds FileStorage::getLastModificationTimestamp() const
    {
        return std::chrono::time_point_cast<std::chrono::nanoseconds>(lastModificationTimestamp).time_since_epoch();
    }

    std::chrono::nanoseconds FileStorage::getLastReloadTimestamp() const
    {
        return lastReloadTimestamp;
    }

    std::chrono::nanoseconds FileStorage::getOpenTimestamp() const
    {
        return openTimestamp;
    }

    std::optional<pair> FileStorage::reload()
    {
        size_t oldFileSize = getSize();
        size_t newFileSize = 0;
        auto fileName = getName();
        {
            struct ::stat tmpFileMetadata;
            auto tmpFile = std::fstream(fileName, std::ios::binary | std::fstream::in | std::fstream::out);
            if (stat(fileName.c_str(), &tmpFileMetadata) == -1)
            {
                throw utils::exception::FeatherPermissionsException(tmpFile, "You don't have permissions to open :", fileName);
            }
            if (lastModificationTimestamp >= std::filesystem::last_write_time(fileName))
            {
                return std::nullopt;
            }
        }
        file.close();
        file = std::fstream(fileName, std::ios::binary | std::fstream::in | std::fstream::out);
        if (!file.good())
        {
            file = std::fstream(fileName, std::ios::binary | std::fstream::in);
            if (!file.good())
            {
                throw utils::exception::FeatherPermissionsException(file, "You don't have permissions to open :", fileName);
            }
        }

        if (stat(fileName.c_str(), &fileMetadata) == -1)
        {
            throw utils::exception::FeatherPermissionsException(file, "You don't have permissions to open :", fileName);
        }

        //Get file size
        file.seekg(0, file.end);
        fileSize = file.tellg();
        newFileSize = fileSize;
        file.seekg(0, file.beg);
        lastModificationTimestamp = std::filesystem::last_write_time(fileName);
        lastReloadTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

        if (!configuration->isHexMode())
        {
            try
            {
                file.imbue(std::locale(configuration->getLocale()));
            }
            catch (std::runtime_error const &e)
            {
                throw utils::exception::FeatherInvalidArgumentException("Error setting locale...");
            }
        }
        return pair(oldFileSize, newFileSize);
    }

    void FileStorage::setCursor(size_t pos)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        if (!isFileExist(fileName))
        {
            throw utils::exception::FeatherFileNotExistException(file, "File :", fileName, "don't exist");
        }
        file.seekp(pos);
        file.seekg(pos);
        clearState();
    }

    void FileStorage::setCursorAtTheEnd()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        file.seekp(0, std::ios::end);
        clearState();
    }

    void FileStorage::setPermissions(const struct stat *otherFileMetadata)
    {
        if (nullptr == otherFileMetadata)
        {
            return;
        }
        if (-1 == chmod(getName().c_str(), otherFileMetadata->st_mode))
        {
            throw utils::exception::FeatherRuntimeException("Could not set permissions for: ", getName());
        }
    }

    void FileStorage::operator<<(char c)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        writeCharacter(c);
    }

    void FileStorage::operator<<(char32_t c)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        writeCharacter(c);
    }

    void FileStorage::operator<<(size_t c)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        writeCharacter(c);
    }

    void FileStorage::operator<<(std::string input)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        size_t currGetPtr = getCurrentPointerPosition();
        file.seekp(0, std::ios::end);
        file << input;
        file.flush();
        file.seekg(currGetPtr);
    }

    void FileStorage::changeName(std::string const &newName)
    {
        if (0 != rename(getName().c_str(), newName.c_str()))
        {
            throw utils::exception::FeatherRuntimeException("Could not change file name from ", getName(), " to ", newName);
        }
        fileName = newName;
    }

    void FileStorage::writeCharacter(char c)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        file.write(&c, 1);
    }

    void FileStorage::writeCharacter(char32_t c)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif

        unsigned short bytesTaken = Utf8Util::getCharacterBytesTaken(utils::helpers::Conversion::char32ToVector(c));
        file.write(reinterpret_cast<char *>(&c), bytesTaken);
    }

    void FileStorage::writeCharacter(size_t c)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        if (!isFileExist(fileName))
        {
            throw utils::exception::FeatherFileNotExistException(file, "File :", fileName, "don't exist");
        }
        file.write((char *)&c, sizeof(size_t));
    }

    void FileStorage::writeChunk(std::vector<char> const &data, size_t size)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        file.write(data.data(), size);
        file.flush();
    }

    const struct stat *FileStorage::getPermissions() const
    {
        if (-1 == stat(fileName.c_str(), &fileMetadata))
        {
            return nullptr;
        }
        return &fileMetadata;
    }

    void FileStorage::readChunk(std::vector<char> &buffer, size_t size, bool alignChunk)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        if (!isFileExist(fileName))
        {
            throw utils::exception::FeatherFileNotExistException(file, "File :", fileName, "don't exist");
        }
        file.read(buffer.data(), size);
        if (alignChunk)
        {
            size_t currCursorPosition = getCurrentPointerPosition();
            size_t maxAlignmentCheck = std::min(2UL, getSize() - currCursorPosition);
            while (utils::Utf8Util::isMiddleOfCharacter(peek(currCursorPosition)) && maxAlignmentCheck--)
            {
                buffer.push_back(peek(currCursorPosition++));
            }
        }
        file.clear();
    }

    void FileStorage::readChunkAsString(std::basic_string<char> &buffer, size_t size, bool alignChunk)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        if (!isFileExist(fileName))
        {
            throw utils::exception::FeatherFileNotExistException(file, "File :", fileName, "don't exist");
        }
        file.read(buffer.data(), size);
        if (alignChunk)
        {
            size_t currCursorPosition = getCurrentPointerPosition();
            size_t maxAlignmentCheck = std::min(2UL, getSize() - currCursorPosition);
            while (utils::Utf8Util::isMiddleOfCharacter(peek(currCursorPosition)) && maxAlignmentCheck--)
            {
                buffer.push_back(peek(currCursorPosition++));
            }
        }
        file.clear();
    }

    std::string FileStorage::getAbsolutePath()
    {
        return filesystem->getAbsolutePath(fileName);
    }

    std::string FileStorage::getLine()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        std::string line;
        std::getline(file, line);
        file.clear();
        return line;
    }

    std::string FileStorage::getPrevLine()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        short newLinesMeet = 0;
        bool fileBegHit = false, endHit = false;
        while (true)
        {
            if (0 == getCurrentPointerPosition())
            {
                if (1 == newLinesMeet)
                {
                    break;
                }
                else if (!fileBegHit)
                {
                    fileBegHit = true;
                    setCursorAtTheEnd();
                    endHit = true;
                    clearState();
                    moveCursorBackward();
                    continue;
                }
                break;
            }
            else if (2 == newLinesMeet || (endHit && 1 == newLinesMeet))
            {
                getCurrentCharacterAndMoveCursor();
                break;
            }

            if (utils::helpers::Lambda::isNewLineChar(getCurrentCharacter()))
            {
                ++newLinesMeet;
                if (2UL == newLinesMeet || (1UL == newLinesMeet && fileBegHit))
                {
                    getCurrentCharacterAndMoveCursor();
                    break;
                }
            }
            moveCursorBackward();
        }
        size_t currCursorPos = getCurrentPointerPosition();
        std::string prevLine = getLine();
        if (file.eof() || getCurrentPointerPosition() == getSize())
        {
            clearState();
        }
        setCursor(currCursorPos);
        return prevLine;
    }

    std::string FileStorage::getName()
    {
        return fileName;
    }

    std::string FileStorage::getPath() const
    {
        return std::string(realpath(fileName.c_str(), nullptr));
    }

    std::fstream &FileStorage::getRawStream()
    {
        return file;
    }

    utils::datatypes::Metadata::MetadataPOD FileStorage::readMetadata()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   {
                                                                       BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0);
                                                                       BOOST_CONTRACT_ASSERT(getSize() >= sizeof(utils::datatypes::Metadata::MetadataPOD));
                                                                   })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((file.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        utils::datatypes::Metadata::MetadataPOD metadata(utils::datatypes::ChangeType::UNKNOWN);
        size_t oldCursor = getCurrentPointerPosition();
        setCursor(0);
        file.read((char *)&metadata, sizeof(utils::datatypes::Metadata::MetadataPOD));
        setCursor(oldCursor);
        return metadata;
    }

    FileStorage::~FileStorage()
    {
    }

    std::unique_ptr<utils::Filesystem> FileStorage::filesystem = std::make_unique<utils::Filesystem>();
} // namespace feather::utils::storage
