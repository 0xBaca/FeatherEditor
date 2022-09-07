#include "utils/storage/FileStorageFactory.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"
#include "utils/storage/FileStorage.hpp"
#include "utils/storage/InMemoryStorage.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class FileStorageTest : public ::testing::Test
{
protected:
    static constexpr const char *testUUID = "220097a9-67fc-4110-b92e-c60227359c68";
    feather::utils::datatypes::Uuid TEST_UUID;
    std::shared_ptr<feather::utils::storage::AbstractStorageFactory> storageFactory;
    std::shared_ptr<feather::utils::storage::AbstractStorageFactory> inMemoryStorageFactory;

public:
    void SetUp() override
    {
        storageFactory = std::make_shared<feather::utils::storage::FileStorageFactory>();
        inMemoryStorageFactory = std::make_shared<feather::utils::storage::InMemoryStorageFactory>();
        TEST_UUID = feather::utils::datatypes::Uuid(testUUID);
    }

    void TearDown() override
    {
    }
};

TEST_F(FileStorageTest, readLineWhenFileWithSingleLine)
{
    // before
    std::string line;
    std::shared_ptr<feather::utils::storage::AbstractStorage> data = storageFactory->getStorage("./tst/ut/helpers/testfiles/searchResultSingleLine", TEST_UUID);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = inMemoryStorageFactory->getStorage("./tst/ut/helpers/testfiles/searchResultSingleLine", TEST_UUID);
    storage->copy(data);

    // when
    line = storage->getLine();

    // then
    ASSERT_STREQ(line.c_str(), "1 1986:24");
}