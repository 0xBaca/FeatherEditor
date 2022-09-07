#include "action/SwitchModeAction.hpp"
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
    class SwitchModeActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
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
    TEST_F(SwitchModeActionIntegTest, swichToTextMode)
    {
        //before
        char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceHexMode(testFile);

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("setCursorInMiddleOfUtf8Character.fea", TEN_BYTE_UTF8_CHARACTER);

        //when
        std::unique_ptr<action::SwitchModeAction> action = std::make_unique<action::SwitchModeAction>(action::SwitchModeActionInput(pair(4UL, 0), TEST_UUID, storage, printingOrchestrator));

        //then
        ASSERT_EQ(action->execute().getResult(), pair(2UL, 0UL));
        ASSERT_FALSE(configuration->isHexMode());

        //when
        configuration->forceHexMode(testFile);
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(1UL, 0UL), THREE_BYTE_UTF8_CHARACTER, 3UL, now);
        action = std::make_unique<action::SwitchModeAction>(action::SwitchModeActionInput(pair(1UL, 2), TEST_UUID, storage, printingOrchestrator));

        //then
        ASSERT_EQ(action->execute().getResult(), pair(1UL, 1UL));
        ASSERT_FALSE(configuration->isHexMode());
    }
    */
} // namespace feather::utils::algorithm