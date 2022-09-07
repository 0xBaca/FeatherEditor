#include "helpers/TestBase.hpp"
#include "mocks/FilesystemMock.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/storage/FileStorageFactory.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::test
{
    std::shared_ptr<feather::utils::storage::AbstractStorage> TestBase::createStorage(std::string &&name, std::vector<char> data)
    {
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = inMemoryStorageFactory->getStorage(std::move(name), TEST_UUID);
        storage->writeChunk(data, data.size());
        return storage;
    }

    std::shared_ptr<feather::utils::storage::AbstractStorage> TestBase::createStorage(std::string &&name, std::shared_ptr<feather::utils::storage::AbstractStorage> otherStorage)
    {
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = inMemoryStorageFactory->getStorage(std::move(name), TEST_UUID);
        storage->copy(otherStorage);
        storage->setCursor(0);
        return storage;
    }

    std::shared_ptr<feather::utils::storage::AbstractStorage> TestBase::createFeatherStorage(std::string &&name, utils::datatypes::ChangeType changeType, size_t noChanges, size_t startPos, unsigned short firstChangeCharacterBytes, std::vector<char> data)
    {
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = inMemoryStorageFactory->getStorage(std::move(name), TEST_UUID);
        storage->overrideMetadata(utils::datatypes::Metadata::MetadataPOD{noChanges, startPos, changeType});
        storage->setCursor(sizeof(utils::datatypes::Metadata::MetadataPOD));
        storage->writeChunk(data, data.size());
        return storage;
    }

    std::shared_ptr<feather::utils::storage::AbstractStorage> TestBase::createEmptyStorage(std::string &&name)
    {
        return inMemoryStorageFactory->getStorage(std::move(name), TEST_UUID);
    }

    void TestBase::removeStorage(std::string &&name)
    {
        inMemoryStorageFactory->removeStorage(std::move(name));
    }

    void TestBase::removeAllStorages()
    {
        inMemoryStorageFactory->removeAllStorages();
    }

    TestBase::TestBase() : FOUR_BYTE_UTF8_CHARACTER{(char)0xF0, (char)0x9F, (char)0x98, (char)0x9A}, FIVE_BYTE_UTF8_CHARACTER{'t', (char)0xC5, (char)0x9B, (char)0xC5, (char)0x9B}, THREE_BYTE_UTF8_CHARACTER{'t', (char)0xC5, (char)0x9B}, TWO_BYTE_UTF8_CHARACTER{(char)0xC5, (char)0x9B}, TEN_BYTE_UTF8_CHARACTER{'t', 'e', (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A}, TWENTY_BYTE_UTF8_CHARACTER{'t', 'e', (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, 't', 'e', (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A}, testFile("test"), TEST_UUID(utils::datatypes::Uuid(testUUID)), TEST_UUID_2(utils::datatypes::Uuid(testUUID2)), TEST_UUID_3(utils::datatypes::Uuid(testUUID3)), TEST_UUID_4(utils::datatypes::Uuid(testUUID4))
    {
        setlocale(LC_ALL, "");
        changesStack = std::make_shared<printer::ChangesStack>();
        filesystemMock = std::make_shared<testing::mocks::FilesystemMock>();
        fileStorageFactory = std::make_shared<feather::utils::storage::FileStorageFactory>();
        inMemoryStorageFactory = std::make_shared<feather::utils::storage::InMemoryStorageFactory>();
        now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        feather::utils::ProgramOptionsParser::clear();
    }

    void TestBase::setMode(utils::FEATHER_MODE mode)
    {
        currentFeatherMode = mode;
    }

    TestBase::~TestBase()
    {
    }
} // namespace feather::test
