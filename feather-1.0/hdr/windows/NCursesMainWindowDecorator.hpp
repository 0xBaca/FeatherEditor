#pragma once

#include "utils/Utf8Util.hpp"
#include "utils/windows/WindowInformationInterface.hpp"
#include "windows/MainWindowInterface.hpp"

namespace feather::windows
{
  class NCursesMainWindowDecorator : public MainWindowInterface
  {
  public:
    NCursesMainWindowDecorator(std::unique_ptr<utils::BufferFillerInterface>, WindowImplInterface *, bool = false, std::optional<utils::datatypes::Uuid> const & = std::nullopt);
    pair getCursorRealPosition(printer::PrintingOrchestratorInterface *) const override;
    pair getLastRealPosition() const override;
    utils::windows::WindowStateContainer getState() const override;
    utils::windows::FramePositions const &getFramePosition() const override;
    std::unique_ptr<utils::BufferFillerInterface> const &getBufferFiller() const override;
    utils::datatypes::Uuid const &getUUID(bool = false) const override;
    std::unique_ptr<utils::windows::WindowInformationInterface> accept(std::shared_ptr<utils::windows::WindowInformationVisitorInterface>) override;
    std::shared_ptr<utils::storage::AbstractStorage> const &getStorage() const override;
    void moveCursor(feather::utils::Direction, std::shared_ptr<utils::windows::WindowInformationVisitorInterface>, std::optional<size_t>) override;
    void showSecondaryWindowCursor(std::pair<pair, pair> const &, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>) override;
    void print(utils::ScreenBuffer const &) override;
    void refreshBuffer(printer::PrintingVisitorInterface *) override;
    void setCursorFromRealPosition(std::pair<size_t, size_t>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::unique_ptr<printer::PrintingVisitorInterface> &, bool = false) override;
    void setFramePosition(std::pair<size_t, size_t>) override;
    void setState(utils::windows::WindowStateContainer const &) override;
    ~NCursesMainWindowDecorator() override;

  private:
    static constexpr size_t HEX_WINDOW_CURSOR_COLOR_PRIORITY = 4096;
    static constexpr size_t STRINGS_COLOR_PRIORITY = 4095;
    std::optional<utils::datatypes::Uuid> siblingUUID;
    std::unique_ptr<utils::BufferFillerInterface> bufferFiller;
    std::pair<pair, pair> secondaryWindowCursor;
    mutable pair lastCursorRealPos;
  };
} // namespace feather::windows
