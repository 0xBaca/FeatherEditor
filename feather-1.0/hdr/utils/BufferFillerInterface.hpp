#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/FilledChunk.hpp"
#include "utils/windows/FramePositions.hpp"
#include "windows/MainWindowInterface.hpp"

namespace feather::utils
{
  class BufferFillerInterface
  {
  private:
    friend class feather::windows::NCursesMainWindowDecorator;
    virtual void setStartFramePosition(pair) = 0;
    virtual void setEndFramePosition(pair) = 0;

  public:
    virtual ScreenBuffer const &getNewWindowBuffer(feather::windows::MainWindowInterface *, FilledChunk, std::shared_ptr<printer::PrintingOrchestratorInterface> = nullptr) = 0;
    virtual ScreenBuffer const &getScreenBuffer() const = 0;
    virtual utils::windows::FramePositions const &getFramePositions() const = 0;
    virtual std::shared_ptr<utils::storage::AbstractStorage> const &getStorage() const = 0;
    virtual void replaceMappedFile(std::shared_ptr<utils::storage::AbstractStorage> &, size_t) = 0;
    virtual ~BufferFillerInterface();
  };
} // namespace feather::utils