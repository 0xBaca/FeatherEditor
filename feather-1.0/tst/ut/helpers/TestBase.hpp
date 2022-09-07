#include "config/Config.hpp"
#include "printer/ChangesStack.hpp"
#include "utils/ScreenBuffer.hpp"
#include "utils/datatypes/ChangeType.hpp"
#include "utils/datatypes/Uuid.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/AbstractStorageFactory.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace feather::test
{
    class TestBase
    {
    protected:
        std::vector<char> FOUR_BYTE_UTF8_CHARACTER;
        std::vector<char> FIVE_BYTE_UTF8_CHARACTER;
        std::vector<char> THREE_BYTE_UTF8_CHARACTER;
        //얘
        std::vector<char> TWO_BYTE_UTF8_CHARACTER;
        // te😚😚
        std::vector<char> TEN_BYTE_UTF8_CHARACTER;
        // te😚😚te😚😚
        std::vector<char> TWENTY_BYTE_UTF8_CHARACTER;
        std::shared_ptr<feather::utils::storage::AbstractStorageFactory> fileStorageFactory;
        std::shared_ptr<feather::utils::storage::AbstractStorageFactory> inMemoryStorageFactory;
        std::shared_ptr<feather::printer::ChangesStack> changesStack;
        std::shared_ptr<utils::FilesystemInterface> filesystemMock;
        inline static const std::string testUUID = "220097a9-67fc-4110-b92e-c60227359c68";
        inline static const std::string testUUID2 = "aaabbbcccddd";
        inline static const std::string testUUID3 = "1111222233334444";
        inline static const std::string testUUID4 = "2222333344445555";
        inline static const int TEST_LOOP_COUNT = 100;
        std::string testFile;
        utils::datatypes::Uuid TEST_UUID;
        utils::datatypes::Uuid TEST_UUID_2;
        utils::datatypes::Uuid TEST_UUID_3;
        utils::datatypes::Uuid TEST_UUID_4;
        std::chrono::nanoseconds now;
        std::shared_ptr<feather::utils::storage::AbstractStorage> createStorage(std::string &&, std::vector<char>);
        std::shared_ptr<feather::utils::storage::AbstractStorage> createStorage(std::string &&, std::shared_ptr<feather::utils::storage::AbstractStorage>);
        std::shared_ptr<feather::utils::storage::AbstractStorage> createFeatherStorage(std::string &&, utils::datatypes::ChangeType, size_t, size_t, unsigned short, std::vector<char>);
        std::shared_ptr<feather::utils::storage::AbstractStorage> createEmptyStorage(std::string &&);
        void removeStorage(std::string &&);
        void removeAllStorages();
        void setMode(utils::FEATHER_MODE);

    public:
        TestBase();
        ~TestBase();
    };
} // namespace feather::test
