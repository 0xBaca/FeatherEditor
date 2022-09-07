#include "action/EditLineBelowAction.hpp"
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
    class EditLineBelowActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
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
/*
    TEST_F(EditLineBelowActionIntegTest, editAtLineBelowWhenNewFile)
    {
        //before
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        text[1UL] = '\n';
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("editAtLineBelowWhenNewFile");

        //when
        std::cout << "T1" << std::endl;
        std::unique_ptr<action::EditLineBelowAction> action = std::make_unique<action::EditLineBelowAction>(action::EditLineBelowActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, nullptr, nullptr));

        //then
        ASSERT_EQ(action->execute().getResult(), pair(0, 0));

        //when
        std::cout << "T2" << std::endl;
        action = std::make_unique<action::EditLineBelowAction>(action::EditLineBelowActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, nullptr, nullptr));
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(0UL, 0UL), FOUR_BYTE_UTF8_CHARACTER, 4UL);

        //then
        ASSERT_EQ(action->execute().getResult(), pair(0, 5UL));

        //when
        action = std::make_unique<action::EditLineBelowAction>(action::EditLineBelowActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, nullptr, nullptr));
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(0, 0), pair(0, 4UL));

        //then
        ASSERT_EQ(action->execute().getResult(), pair(0, 5UL));
    }

    TEST_F(EditLineBelowActionIntegTest, afterFileEndWithoutNewLine)
    {
        //before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("afterFileEndWithoutNewLine", TEN_BYTE_UTF8_CHARACTER);

        //when
        std::unique_ptr<action::EditLineBelowAction> action = std::make_unique<action::EditLineBelowAction>(action::EditLineBelowActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, nullptr, nullptr));

        //then
        ASSERT_EQ(action->execute().getResult(), pair(10UL, 0));
    }
    */
} // namespace feather::utils::algorithm