#pragma once

#include "utils/storage/AbstractStorage.hpp"

#include <filesystem>
#include <fstream>

namespace feather::utils::storage
{
  class FileStorage : public AbstractStorage
  {
  public:
    explicit FileStorage(std::string const &, bool = false) noexcept(false);
    inline static bool isFileExist(std::string const &);
    bool endOfStream() override;
    bool isFirstLine() override;
    bool isLastLine() override;
    bool isLastPosition(size_t) override;
    bool isBeginingOfCharacter(size_t) override;
    bool isValidFeatherStorage() override;
    bool wasFileCreated() const override;
    char peek() override;
    char peek(size_t) override;
    char32_t getCharacterAtPos(size_t) override;
    char32_t getCurrentCharacter() override;
    char32_t getCurrentCharacterAndMoveCursor() override;
    size_t alignToProperPosition(size_t) override;
    size_t getCurrentPointerPosition() override;
    size_t getPreviousCharacterPosition(size_t) override;
    size_t getPreviousCharacterPosition(size_t, size_t) override;
    size_t getBegining() const override;
    size_t getEnd() const override;
    size_t getSize() override;
    size_t moveCursorBackward() override;
    size_t moveCursorToLineBegining(utils::Direction) override;
    void appendToBegining(std::vector<char> &, std::shared_ptr<AbstractStorage>) override;
    void copy(std::shared_ptr<feather::utils::storage::AbstractStorage>) override;
    size_t copy(std::shared_ptr<feather::utils::storage::AbstractStorage>, size_t) override;
    void clearState() override;
    void copy(std::vector<char32_t> &) override;
    void overrideMetadata(utils::datatypes::Metadata::MetadataPOD &&) override;
    std::optional<pair> reload() override;
    std::chrono::nanoseconds getLastModificationTimestamp() const override;
    std::chrono::nanoseconds getLastReloadTimestamp() const override;
    std::chrono::nanoseconds getOpenTimestamp() const override;
    void setCursor(size_t) override;
    void setCursorAtTheEnd() override;
    void setPermissions(const struct stat *) override;
    void operator<<(char) override;
    void operator<<(char32_t) override;
    void operator<<(size_t) override;
    void operator<<(std::string) override;
    void changeName(std::string const &) override;
    void writeCharacter(char) override;
    void writeCharacter(char32_t) override;
    void writeCharacter(size_t) override;
    void writeChunk(std::vector<char> const &, size_t) override;
    std::string getAbsolutePath() override;
    std::string getLine() override;
    std::string getPrevLine() override;
    std::string getName() override;
    std::string getPath() const override;
    std::fstream &getRawStream() override;
    const struct stat *getPermissions() const override;
    void readChunk(std::vector<char> &, size_t, bool = true) override;
    void readChunkAsString(std::basic_string<char> &, size_t, bool = true) override;
    utils::datatypes::Metadata::MetadataPOD readMetadata() override;
    ~FileStorage();

  private:
    mutable struct ::stat fileMetadata;
    mutable std::fstream file;
    std::string fileName;
    std::chrono::nanoseconds lastReloadTimestamp;
    std::chrono::nanoseconds openTimestamp;
    size_t fileSize;
    bool fileWasCreated;
    static std::unique_ptr<utils::Filesystem> filesystem;
    std::filesystem::file_time_type lastModificationTimestamp;
  };
} // namespace feather::utils::storage
