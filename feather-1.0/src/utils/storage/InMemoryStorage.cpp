#include "config/Config.hpp"
#include "utils/helpers/Lambda.hpp"
#include "utils/exception/FeatherMemoryException.hpp"
#include "utils/storage/InMemoryStorage.hpp"
#include "utils/Utf8Util.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::utils::storage
{
    InMemoryStorage::InMemoryStorage(std::string const nameArg, bool chekcUtf8Compatibility)
        : name(nameArg), forceReload(false)
    {
        lastModificationTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        lastReloadTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
    }

    InMemoryStorage::InMemoryStorage(std::string const nameArg, std::basic_iostream<char> &streamArg, std::chrono::nanoseconds lastModififcationTimestampArg)
        : name(nameArg), forceReload(false)
    {
        lastModificationTimestamp = lastModififcationTimestampArg;
        lastReloadTimestamp = openTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        streamArg.seekg(0);
        streamArg.seekp(0);
        stream << streamArg.rdbuf();
        streamArg.seekp(0);
        streamArg.seekg(0);
        stream.seekp(0);
        stream.seekg(0);
    }

    bool InMemoryStorage::endOfStream()
    {
        return getCurrentPointerPosition() == getSize();
    }

    bool InMemoryStorage::isFirstLine()
    {
        return !getCurrentPointerPosition();
    }

    bool InMemoryStorage::isLastLine()
    {
        size_t savedPosition = getCurrentPointerPosition();
        size_t noNewLinesMeet = 0UL;
        while (noNewLinesMeet != 2UL)
        {
            if (getSize() == getCurrentPointerPosition() || stream.eof())
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

    bool InMemoryStorage::isLastPosition(size_t pos)
    {
        return (!pos && !getSize()) || (pos == getSize());
    }

    bool InMemoryStorage::isBeginingOfCharacter(size_t pos)
    {
        size_t currPos = getCurrentPointerPosition();
        setCursor(pos);
        bool result = Utf8Util::isBeginingOfCharacter(stream.peek());
        setCursor(currPos);
        return result;
    }

    bool InMemoryStorage::isValidFeatherStorage()
    {
        return getSize() >= sizeof(utils::datatypes::Metadata::MetadataPOD);
    }

    bool InMemoryStorage::wasFileCreated() const
    {
        return false;
    }

    char InMemoryStorage::peek()
    {
        int peekedCharacter = stream.peek();
        char toReturn = (-1 == peekedCharacter) ? 0 : peekedCharacter;
        stream.clear();
        return toReturn;
    }

    char InMemoryStorage::peek(size_t pos)
    {
        size_t curPos = getCurrentPointerPosition();
        setCursor(pos);
        int peekedCharacter = stream.peek();
        char toReturn = (-1 == peekedCharacter) ? 0 : peekedCharacter;
        setCursor(curPos);
        stream.clear();
        return toReturn;
    }

    char32_t InMemoryStorage::getCharacterAtPos(size_t pos)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check streamStateIsGood = boost::contract::function()
                                                       .precondition([&]
                                                                     { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                       .postcondition([&]
                                                                      { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
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

    char32_t InMemoryStorage::getCurrentCharacter()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check stateIsOK = boost::contract::function()
                                               .precondition([&]
                                                             { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                               .postcondition([&]
                                                              { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        size_t currPos = getCurrentPointerPosition();
        std::vector<char> chunk(4UL, 0);
        readChunk(chunk, std::min(4UL, getSize() - currPos));

        unsigned short length = Utf8Util::getCharacterBytesTaken(chunk);
        chunk.resize(length);
        setCursor(currPos);
        return utils::helpers::Conversion::vectorOfCharToChar32_t(chunk);
    }

    char32_t InMemoryStorage::getCurrentCharacterAndMoveCursor()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check stateIsOK = boost::contract::function()
                                               .precondition([&]
                                                             { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                               .postcondition([&]
                                                              { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
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

    size_t InMemoryStorage::getCurrentPointerPosition()
    {
        clearState();
#ifdef _DEBUG_MODE_
        boost::contract::check streamStateIsGood = boost::contract::function()
                                                       .precondition([&]
                                                                     { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit | std::fstream::badbit)) == 0); });
#endif
        return static_cast<size_t>(stream.tellg());
    }

    std::chrono::nanoseconds InMemoryStorage::getLastModificationTimestamp() const
    {
        return lastModificationTimestamp;
    }

    size_t InMemoryStorage::alignToProperPosition(size_t position)
    {
        if (getSize() < position)
        {
            return getSize();
        }
        while (!isBeginingOfCharacter(position))
        {
            --position;
        }
        return position;
    }

    size_t InMemoryStorage::getPreviousCharacterPosition(size_t pos)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
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

    size_t InMemoryStorage::getPreviousCharacterPosition(size_t pos, size_t offset)
    {
        while (offset--)
        {
            pos = getPreviousCharacterPosition(pos);
        }
        return pos;
    }

    size_t InMemoryStorage::getBegining() const
    {
        return stream.beg;
    }

    size_t InMemoryStorage::getEnd() const
    {
        return stream.end;
    }

    size_t InMemoryStorage::getSize()
    {
        size_t currPos = getCurrentPointerPosition();
        stream.seekp(0, std::ios::end);
        stream.seekg(0, std::ios::end);
        size_t size = getCurrentPointerPosition();
        setCursor(currPos);
        return size;
    }

    size_t InMemoryStorage::moveCursorBackward()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check stateIsOK = boost::contract::function()
                                               .precondition([&]
                                                             { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                               .postcondition([&]
                                                              { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        stream.seekg(-1, stream.cur);
        if (configuration->isHexMode())
        {
            return 1;
        }

        while (Utf8Util::isMiddleOfCharacter((char)stream.peek()))
        {
            stream.seekg(-1, stream.cur);
        }
        setCursor(stream.tellg());
        size_t currPos = getCurrentPointerPosition();
        std::vector<char> chunk(4UL, 0);
        readChunk(chunk, std::min(4UL, getSize() - currPos));
        unsigned short bytesTaken = Utf8Util::getCharacterBytesTaken(chunk);
        setCursor(currPos);
        return bytesTaken;
    }

    size_t InMemoryStorage::moveCursorToLineBegining(utils::Direction direction)
    {
        while (getCurrentPointerPosition() && getCharacterAtPos(getCurrentPointerPosition()) != '\n')
        {
            moveCursorBackward();
        }
        if (U'\n' == getCharacterAtPos(getCurrentPointerPosition()))
        {
            getCurrentCharacterAndMoveCursor();
        }
        return getCurrentPointerPosition();
    }

    void InMemoryStorage::appendToBegining(std::vector<char> &data, std::shared_ptr<AbstractStorage> tmpStorage)
    {
        setCursor(0);
        tmpStorage->writeChunk(data, data.size());
        std::vector<char> oldFileBuffer(configuration->getMemoryBytesRelaxed(), 0);
        for (size_t currFilePos = 0; currFilePos < getSize();)
        {
            size_t canRead = std::min(getSize() - currFilePos, configuration->getMemoryBytesRelaxed());
            if (canRead != oldFileBuffer.size())
            {
                oldFileBuffer.resize(canRead);
            }
            readChunk(oldFileBuffer, oldFileBuffer.size(), false);
            tmpStorage->setCursorAtTheEnd();
            tmpStorage->writeChunk(oldFileBuffer, oldFileBuffer.size());
            currFilePos += oldFileBuffer.size();
        }
    }

    void InMemoryStorage::clearState()
    {
        stream.clear();
    }

    void InMemoryStorage::copy(std::shared_ptr<utils::storage::AbstractStorage> from)
    {
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

    void InMemoryStorage::copy(std::vector<char32_t> &from)
    {
        for (char32_t v : from)
        {
            *this << v;
        }
    }

    size_t InMemoryStorage::copy(std::shared_ptr<utils::storage::AbstractStorage> from, size_t bytesToCopy)
    {
        std::vector<char> buffer(configuration->getMemoryBytesRelaxed() >= bytesToCopy ? bytesToCopy : configuration->getMemoryBytesRelaxed(), 0);
        from->readChunk(buffer, buffer.size());
        writeChunk(buffer, buffer.size());
        return buffer.size();
    }

    void InMemoryStorage::overrideMetadata(utils::datatypes::Metadata::MetadataPOD &&metadata)
    {
        size_t oldCursor = getCurrentPointerPosition();
        setCursor(0);
        stream.write((char *)&metadata, sizeof(utils::datatypes::Metadata::MetadataPOD));
        size = (size < sizeof(utils::datatypes::Metadata::MetadataPOD)) ? sizeof(utils::datatypes::Metadata::MetadataPOD) : size;
        setCursor(oldCursor);
    }

    std::chrono::nanoseconds InMemoryStorage::getLastReloadTimestamp() const
    {
        return lastReloadTimestamp;
    }

    std::chrono::nanoseconds InMemoryStorage::getOpenTimestamp() const
    {
        return openTimestamp;
    }

    std::optional<pair> InMemoryStorage::reload()
    {
        if (forceReload)
        {
            size_t oldSize = size;
            forceReload = false;
            lastReloadTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
            lastModificationTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
            stream.seekg(0);
            stream.seekp(0);
            size = getSize();
            return pair(oldSize, size);
        }
        return std::nullopt;
    }

    void InMemoryStorage::setCursor(size_t pos)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check stateIsOK = boost::contract::function()
                                               .precondition([&]
                                                             { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        stream.seekp(pos);
        stream.seekg(pos);
        clearState();
    }

    void InMemoryStorage::setCursorAtTheEnd()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check stateIsOK = boost::contract::function()
                                               .precondition([&]
                                                             { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                               .postcondition([&]
                                                              { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        stream.seekp(0, std::ios::end);
        stream.seekg(0, std::ios::end);
        clearState();
    }

    void InMemoryStorage::setPermissions(const struct stat *otherFileMetadata)
    {
    }

    void InMemoryStorage::operator<<(char c)
    {
        lastModificationTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        stream.seekp(0, std::ios::end);
        writeCharacter(c);
    }

    void InMemoryStorage::operator<<(char32_t c)
    {
        lastModificationTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        stream.seekp(0, std::ios::end);
        writeCharacter(c);
    }

    void InMemoryStorage::operator<<(size_t c)
    {
        lastModificationTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        stream.seekp(0, std::ios::end);
        writeCharacter(c);
    }

    void InMemoryStorage::operator<<(std::string input)
    {
        lastModificationTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        size_t currGetPtr = getCurrentPointerPosition();
        stream.seekp(0, std::ios::end);
        stream << input;
        stream.flush();
        stream.seekg(currGetPtr);
    }

    void InMemoryStorage::changeName(std::string const &newName)
    {
        name = newName;
    }

    void InMemoryStorage::writeCharacter(char c)
    {
        stream << c;
        stream.flush();
        if (stream.fail())
        {
            throw utils::exception::FeatherMemoryException("Error writing to disk...");
        }
        clearState();
        setCursor(stream.tellp());
    }

    void InMemoryStorage::writeCharacter(char32_t c)
    {
        unsigned short cCLen = Utf8Util::getCharacterBytesTakenLight(c);
        stream.write((char *)&c, cCLen);
        if (stream.fail())
        {
            throw utils::exception::FeatherMemoryException("Error writing to disk...");
        }
        setCursor(stream.tellp());
    }

    void InMemoryStorage::writeCharacter(size_t c)
    {
        stream.write((char *)&c, sizeof(size_t));
        if (stream.fail())
        {
            throw utils::exception::FeatherMemoryException("Error writing to disk...");
        }
        setCursor(stream.tellp());
    }

    void InMemoryStorage::writeChunk(std::vector<char> const &data, size_t size)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); })
                                                     .postcondition([&]
                                                                    { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        stream.write(data.data(), size);
        stream.flush();
    }

    const struct stat *InMemoryStorage::getPermissions() const
    {
        return nullptr;
    }

    std::string InMemoryStorage::getAbsolutePath()
    {
        return {};
    }

    std::string InMemoryStorage::getLine()
    {
        std::string line;
        std::getline(stream, line);
        //Some eof bit will also casue failbit
        stream.clear();
        return line;
    }

    std::string InMemoryStorage::getPrevLine()
    {
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
        if (stream.eof() || getCurrentPointerPosition() == getSize())
        {
            clearState();
        }
        setCursor(currCursorPos);
        return prevLine;
    }

    std::string InMemoryStorage::getName()
    {
        return name;
    }

    std::string InMemoryStorage::getPath() const
    {
        return {};
    }

    std::stringstream &InMemoryStorage::getRawStream()
    {
        return stream;
    }

    void InMemoryStorage::readChunk(std::vector<char> &buffer, size_t size, bool alignChunk)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        stream.read(buffer.data(), size);
        if (alignChunk)
        {
            size_t currCursorPosition = getCurrentPointerPosition();
            while (utils::Utf8Util::isMiddleOfCharacter(peek(currCursorPosition)))
            {
                buffer.push_back(peek(currCursorPosition++));
            }
        }
        stream.clear();
    }

    void InMemoryStorage::readChunkAsString(std::basic_string<char> &buffer, size_t size, bool alignChunk)
    {
#ifdef _DEBUG_MODE_
        boost::contract::check fileStateIsGood = boost::contract::function()
                                                     .precondition([&]
                                                                   { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        stream.read(buffer.data(), size);
        if (alignChunk)
        {
            size_t currCursorPosition = getCurrentPointerPosition();
            while (utils::Utf8Util::isMiddleOfCharacter(peek(currCursorPosition)))
            {
                buffer.push_back(peek(currCursorPosition++));
            }
        }
        stream.clear();
    }

    utils::datatypes::Metadata::MetadataPOD InMemoryStorage::readMetadata()
    {
#ifdef _DEBUG_MODE_
        boost::contract::check stateIsOK = boost::contract::function()
                                               .precondition([&]
                                                             {
                                                                 BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0);
                                                                 BOOST_CONTRACT_ASSERT(getSize() >= sizeof(utils::datatypes::Metadata::MetadataPOD));
                                                             })
                                               .postcondition([&]
                                                              { BOOST_CONTRACT_ASSERT((stream.rdstate() & (std::fstream::failbit ^ std::fstream::badbit)) == 0); });
#endif
        utils::datatypes::Metadata::MetadataPOD metadata(utils::datatypes::ChangeType::UNKNOWN);
        size_t oldCursor = getCurrentPointerPosition();
        setCursor(0);
        stream.read((char *)&metadata, sizeof(utils::datatypes::Metadata::MetadataPOD));
        setCursor(oldCursor);
        return metadata;
    }

    void InMemoryStorage::mockReloadStorageContent(std::vector<char> const &newContent)
    {
        forceReload = true;
        stream.str("");
        for (auto &a : newContent)
        {
            stream << a;
        }
    }

    InMemoryStorage::~InMemoryStorage()
    {
    }
} // namespace feather::utils::storage
