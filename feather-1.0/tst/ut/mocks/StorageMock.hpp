#pragma once

#include "printer/PrintingOrchestrator.hpp"
#include "utils/storage/AbstractStorage.hpp"

#include <gmock/gmock.h>

using namespace feather::utils::storage;

namespace testing::mocks
{
  class StorageMock : public AbstractStorage
  {
  public:
    MOCK_CONST_METHOD0(getBegining, size_t());
    MOCK_CONST_METHOD0(getEnd, size_t());
    MOCK_CONST_METHOD0(getLastModificationTimestamp, std::chrono::nanoseconds());
    MOCK_CONST_METHOD0(getLastReloadTimestamp, std::chrono::nanoseconds());
    MOCK_CONST_METHOD0(getOpenTimestamp, std::chrono::nanoseconds());
    MOCK_CONST_METHOD0(getPath, std::string());
    MOCK_CONST_METHOD0(getPermissions, const struct ::stat *());
    MOCK_CONST_METHOD0(wasFileCreated, bool());
    MOCK_METHOD0(clearState, void());
    MOCK_METHOD0(endOfStream, bool());
    MOCK_METHOD0(getAbsolutePath, std::string());
    MOCK_METHOD0(getCurrentCharacter, char32_t());
    MOCK_METHOD0(getCurrentCharacterAndMoveCursor, char32_t());
    MOCK_METHOD0(getCurrentPointerPosition, size_t());
    MOCK_METHOD0(getLine, std::string());
    MOCK_METHOD0(getName, std::string());
    MOCK_METHOD0(getPrevLine, std::string());
    MOCK_METHOD0(getRawStream, std::basic_iostream<char> &());
    MOCK_METHOD0(getSize, size_t());
    MOCK_METHOD0(isFirstLine, bool());
    MOCK_METHOD0(isLastLine, bool());
    MOCK_METHOD0(isValidFeatherStorage, bool());
    MOCK_METHOD0(moveCursorBackward, size_t());
    MOCK_METHOD0(peek, char());
    MOCK_METHOD0(readMetadata, feather::utils::datatypes::Metadata::MetadataPOD());
    MOCK_METHOD0(reload, std::optional<feather::pair>());
    MOCK_METHOD0(setCursorAtTheEnd, void());
    MOCK_METHOD1(alignToProperPosition, size_t(size_t));
    MOCK_METHOD1(changeName, void(std::string const &));
    MOCK_METHOD1(copy, void(std::shared_ptr<AbstractStorage>));
    MOCK_METHOD1(copy, void(std::vector<char32_t> &));
    MOCK_METHOD1(getCharacterAtPos, char32_t(size_t));
    MOCK_METHOD1(getNumberofCharacterThatCanBeTaken, size_t(size_t));
    MOCK_METHOD1(getPreviousCharacterPosition, size_t(size_t));
    MOCK_METHOD1(isBeginingOfCharacter, bool(size_t));
    MOCK_METHOD1(isLastPosition, bool(size_t));
    MOCK_METHOD1(moveCursorForward, void(int));
    MOCK_METHOD1(moveCursorToLineBegining, size_t(feather::utils::Direction));
    MOCK_METHOD1(peek, char(size_t));
    MOCK_METHOD1(setCursor, void(size_t));
    MOCK_METHOD1(setCursorAtNextPosAfter, void(size_t));
    MOCK_METHOD1(setPermissions, void(const struct ::stat *));
    MOCK_METHOD1(skipCharacter, size_t(size_t));
    MOCK_METHOD1(writeCharacter, void(char));
    MOCK_METHOD1(writeCharacter, void(char32_t));
    MOCK_METHOD1(writeCharacter, void(size_t));
    MOCK_METHOD2(appendToBegining, void(std::vector<char> &, std::shared_ptr<AbstractStorage>));
    MOCK_METHOD2(copy, size_t(std::shared_ptr<AbstractStorage>, size_t));
    MOCK_METHOD2(getPreviousCharacterPosition, size_t(size_t, size_t));
    MOCK_METHOD2(writeChunk, void(std::vector<char> const &, size_t));
    MOCK_METHOD3(readChunk, void(std::vector<char> &, size_t, bool));
    MOCK_METHOD3(readChunkAsString, void(std::basic_string<char> &, size_t, bool));
    virtual void operator<<(char) {}
    virtual void operator<<(char32_t) {}
    virtual void operator<<(size_t) {}
    virtual void operator<<(std::string) {}
    void overrideChangeMetadata(feather::utils::datatypes::ChangePOD &&arg, size_t) {}
    void overrideMetadata(feather::utils::datatypes::Metadata::MetadataPOD &&arg) {}
  };
} // namespace testing::mocks
