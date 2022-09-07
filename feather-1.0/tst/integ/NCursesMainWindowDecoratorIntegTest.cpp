#include "helpers/TestBase.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/PrintingStorageMock.hpp"
#include "mocks/WindowsFactoryMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/MemoryPrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/NCursesWrapper.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/logger/policy/MemoryLogPolicy.hpp"
#include "windows/NCursesWindow.hpp"
#include "windows/NCursesWindowsFactory.hpp"
#include "windows/WindowsManager.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::windows
{
  class NCursesMainWindowDecoratorIntegTest : public ::testing::Test, public ::feather::test::TestBase
  {
  protected:
    std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
    std::unique_ptr<utils::BufferFillerInterface> bufferFillerMock;
    std::shared_ptr<PrintingStorageInterface> filePrintingStorage;
    std::shared_ptr<PrintingOrchestrator> orchestrator;

  public:
    void SetUp() override
    {
      bufferFillerMock = std::make_unique<BufferFillerMock>();
      filePrintingStorage = std::make_shared<FilePrintingStorage>(inMemoryStorageFactory);
      printingOrchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
    }

    void TearDown() override
    {
      printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
    }
  };
}