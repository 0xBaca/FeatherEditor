#include "action/MoveToByteAction.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/algorithm/FastSearch.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Conversion.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/FileStorageFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::utils::algorithm
{
class MoveToByteActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
{
protected:
    std::unique_ptr<utils::BufferFillerInterface> bufferFillerMock;
    std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
    std::shared_ptr<PrintingStorageInterface> filePrintingStorage;

public:
    void SetUp() override
    {
        bufferFillerMock = std::unique_ptr<BufferFillerMock>(new BufferFillerMock());
        filePrintingStorage = std::make_shared<FilePrintingStorage>(inMemoryStorageFactory);
        printingOrchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
    }

    void TearDown() override
    {
        printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
    }
};

TEST_F(MoveToByteActionIntegTest, setCursorInMiddleOfUtf8Character)
{
    //before
    const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
    utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
    configuration->overrideConfiguration();
    
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("setCursorInMiddleOfUtf8Character.fea", TEN_BYTE_UTF8_CHARACTER);

    //when
    std::unique_ptr<action::MoveToByteAction> action = std::make_unique<action::MoveToByteAction>(action::MoveToByteActionInput(3UL, TEST_UUID, storage, printingOrchestrator));

    //then
    ASSERT_EQ(action->execute().getResult(), pair(2, 0));

    //when
    action = std::make_unique<action::MoveToByteAction>(action::MoveToByteActionInput(4UL, TEST_UUID, storage, printingOrchestrator));

    //then
    ASSERT_EQ(action->execute().getResult(), pair(2, 0));

    //when
    action = std::make_unique<action::MoveToByteAction>(action::MoveToByteActionInput(5UL, TEST_UUID, storage, printingOrchestrator));

    //then
    ASSERT_EQ(action->execute().getResult(), pair(2, 0));

    //when
    action = std::make_unique<action::MoveToByteAction>(action::MoveToByteActionInput(6UL, TEST_UUID, storage, printingOrchestrator));

    //then
    ASSERT_EQ(action->execute().getResult(), pair(6, 0));
}
}