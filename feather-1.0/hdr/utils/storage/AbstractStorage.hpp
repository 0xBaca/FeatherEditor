#pragma once

#include "utils/Direction.hpp"
#include "utils/datatypes/Global.hpp"
#include "utils/datatypes/Metadata.hpp"

#include <sys/stat.h>

#include <optional>

namespace feather::utils::storage
{
    struct AbstractStorage
    {
    protected:
        bool checkIfUtf8Storage(AbstractStorage *);
        bool isCurrCharacterNewLine();

    public:
        virtual bool endOfStream() = 0;
        virtual bool isFirstLine() = 0;
        virtual bool isLastLine() = 0;
        virtual bool isLastPosition(size_t) = 0;
        virtual bool isBeginingOfCharacter(size_t) = 0;
        virtual bool isValidFeatherStorage() = 0;
        virtual bool wasFileCreated() const = 0;
        virtual char peek() = 0;
        virtual char peek(size_t) = 0;
        virtual char32_t getCharacterAtPos(size_t) = 0;
        virtual char32_t getCurrentCharacter() = 0;
        virtual char32_t getCurrentCharacterAndMoveCursor() = 0;
        virtual size_t alignToProperPosition(size_t) = 0;
        virtual size_t getCurrentPointerPosition() = 0;
        virtual size_t getPreviousCharacterPosition(size_t) = 0;
        virtual size_t getPreviousCharacterPosition(size_t, size_t) = 0;
        virtual size_t getBegining() const = 0;
        virtual size_t getEnd() const = 0;
        virtual size_t getSize() = 0;
        virtual size_t moveCursorBackward() = 0;
        virtual size_t moveCursorToLineBegining(utils::Direction) = 0;
        virtual void appendToBegining(std::vector<char> &, std::shared_ptr<AbstractStorage>) = 0;
        virtual void clearState() = 0;
        virtual void copy(std::shared_ptr<utils::storage::AbstractStorage>) = 0;
        virtual size_t copy(std::shared_ptr<utils::storage::AbstractStorage>, size_t) = 0;
        virtual void copy(std::vector<char32_t> &) = 0;
        virtual void overrideMetadata(utils::datatypes::Metadata::MetadataPOD &&) = 0;
        virtual std::optional<pair> reload() = 0;
        virtual std::chrono::nanoseconds getLastModificationTimestamp() const = 0;
        virtual std::chrono::nanoseconds getLastReloadTimestamp() const = 0;
        virtual std::chrono::nanoseconds getOpenTimestamp() const = 0;
        virtual void setCursor(size_t) = 0;
        virtual void setCursorAtTheEnd() = 0;
        virtual void setPermissions(const struct stat *) = 0;
        virtual void operator<<(char) = 0;
        virtual void operator<<(char32_t) = 0;
        virtual void operator<<(size_t) = 0;
        virtual void operator<<(std::string) = 0;
        virtual void changeName(std::string const &) = 0;
        virtual void writeCharacter(char) = 0;
        virtual void writeCharacter(char32_t) = 0;
        virtual void writeCharacter(size_t) = 0;
        virtual void writeChunk(std::vector<char> const &, size_t) = 0;
        virtual std::string getAbsolutePath() = 0;
        virtual std::string getLine() = 0;
        virtual std::string getPrevLine() = 0;
        virtual std::string getName() = 0;
        virtual std::string getPath() const = 0;
        virtual std::basic_iostream<char> &getRawStream() = 0;
        virtual void readChunk(std::vector<char> &, size_t, bool = true) = 0;
        virtual void readChunkAsString(std::basic_string<char> &, size_t, bool = true) = 0;
        virtual utils::datatypes::Metadata::MetadataPOD readMetadata() = 0;
        virtual const struct stat *getPermissions() const = 0;
        virtual ~AbstractStorage() = 0;
    };
} // namespace feather::utils::storage
