#include "utils/BufferFillerInterface.hpp"

#include <gmock/gmock.h>

using namespace feather::utils;

namespace testing::mocks
{
  class BufferFillerMock : public BufferFillerInterface
  {
  public:
    MOCK_CONST_METHOD0(getFramePositions, feather::utils::windows::FramePositions const &());
    MOCK_CONST_METHOD0(getScreenBuffer, feather::utils::ScreenBuffer const &());
    MOCK_CONST_METHOD0(getStorage, std::shared_ptr<feather::utils::storage::AbstractStorage> const &());
    MOCK_METHOD1(setEndFramePosition, void(feather::pair));
    MOCK_METHOD1(setStartFramePosition, void(feather::pair));
    MOCK_METHOD2(replaceMappedFile, void(std::shared_ptr<feather::utils::storage::AbstractStorage> &, size_t));
    MOCK_METHOD3(getNewWindowBuffer, ScreenBuffer const &(feather::windows::MainWindowInterface *, feather::utils::FilledChunk, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>));
  };
} // namespace testing::mocks