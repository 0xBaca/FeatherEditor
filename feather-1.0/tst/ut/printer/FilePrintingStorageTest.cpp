#include "config/Config.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace feather::printer;

extern std::shared_ptr<const feather::config::Configuration> configuration;

namespace feather::printer
{
    class FilePrintingStorageTest : public ::testing::Test
    {
    protected:
        std::string testFile;
        static constexpr const char *testUUID = "220097a9-67fc-4110-b92e-c60227359c68";
        utils::datatypes::Uuid TEST_UUID;
        std::shared_ptr<feather::utils::storage::AbstractStorageFactory> storageFactory;

    public:
        void SetUp() override
        {
            testFile = "testFile";
            configuration->forceTextMode(testFile);
            TEST_UUID = utils::datatypes::Uuid(testUUID);
            storageFactory = std::make_shared<feather::utils::storage::InMemoryStorageFactory>();
        }

        void TearDown() override
        {
            storageFactory->removeAllStorages();
        }
    };

    TEST_F(FilePrintingStorageTest, addCharacterToChanges)
    {
        // before
        FilePrintingStorage fps(storageFactory);
        std::vector<char> changes = {(char)0xF0, (char)0x9F, (char)0x98, (char)0x9A};

        // when
        fps.addToNewChanges(TEST_UUID, pair(0, 0), changes, 4UL);

        // then
        auto result = fps.getNewChanges(TEST_UUID, 0, 0, 1024);
        ASSERT_EQ(result.size(), 4UL);
        ASSERT_EQ(result[0], '\xF0');
        ASSERT_EQ(result[1], '\x9F');
        ASSERT_EQ(result[2], '\x98');
        ASSERT_EQ(result[3], '\x9A');
    }

    TEST_F(FilePrintingStorageTest, addCharacterToChangesWhenPresent)
    {
        // before
        FilePrintingStorage fps(storageFactory);
        std::vector<char> changes = {(char)0xF0, (char)0x9F, (char)0x98, (char)0x9A};

        // when
        fps.addToNewChanges(TEST_UUID, pair(0, 0), changes, 4UL);
        fps.addToNewChanges(TEST_UUID, pair(0, 4UL), changes, 4UL);

        // then
        auto result = fps.getNewChanges(TEST_UUID, 0, 0, 1024UL);
        ASSERT_EQ(result.size(), 8UL);
        ASSERT_EQ(result, (std::vector<char>{(char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A}));
    }

    TEST_F(FilePrintingStorageTest, addCharacterToChangesWhenPresent_2)
    {
        // before
        FilePrintingStorage fps(storageFactory);
        std::vector<char> changes = {(char)0xE2, (char)0x88, (char)0x91};

        // when
        fps.addToNewChanges(TEST_UUID, pair(0, 0), changes, 3UL);
        fps.addToNewChanges(TEST_UUID, pair(0, 0), changes, 3UL);
        fps.addToNewChanges(TEST_UUID, pair(0, 0), changes, 3UL);
        fps.addToNewChanges(TEST_UUID, pair(0, 0), changes, 3UL);

        // then
        auto result = fps.getNewChanges(TEST_UUID, 0, 0, 1024);
        ASSERT_EQ(result.size(), 12UL);
        ASSERT_EQ(result[0], '\xE2');
        ASSERT_EQ(result[1], '\x88');
        ASSERT_EQ(result[2], '\x91');
        ASSERT_EQ(result[3], '\xE2');
        ASSERT_EQ(result[4], '\x88');
        ASSERT_EQ(result[5], '\x91');
        ASSERT_EQ(result[6], '\xE2');
        ASSERT_EQ(result[7], '\x88');
        ASSERT_EQ(result[8], '\x91');
        ASSERT_EQ(result[9], '\xE2');
        ASSERT_EQ(result[10], '\x88');
        ASSERT_EQ(result[11], '\x91');
    }

    TEST_F(FilePrintingStorageTest, getChangesFile)
    {
        // before
        FilePrintingStorage fps(storageFactory);

        // when
        auto storage = fps.getChangesFile(2UL, "testFile", TEST_UUID);

        // then
        ASSERT_EQ(storage.second->getCurrentPointerPosition(), 0);
    }

    TEST_F(FilePrintingStorageTest, getChanges)
    {
        // before
        FilePrintingStorage fps(storageFactory);

        // when
        fps.addToNewChanges(TEST_UUID, pair(0, 0), std::vector<char>{'t'}, 1UL);
        fps.addToNewChanges(TEST_UUID, pair(0, 1), std::vector<char>{'e'}, 1UL);
        fps.addToNewChanges(TEST_UUID, pair(0, 2), std::vector<char>{'s'}, 1UL);
        fps.addToNewChanges(TEST_UUID, pair(0, 3), std::vector<char>{'t'}, 1UL);
        std::vector<char> changes = fps.getNewChanges(TEST_UUID, 0, 2, 2);

        // then
        ASSERT_THAT(changes.size(), 2);
        ASSERT_EQ(changes[0], 's');
        ASSERT_EQ(changes[1], 't');
    }
}
