#pragma once

#include "printer/PrintingVisitorInterface.hpp"
#include "utils/Direction.hpp"
#include "utils/datatypes/Colors.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/windows/WindowInformationVisitorInterface.hpp"
#include "utils/windows/WindowStateContainer.hpp"
#include "windows/WindowImplInterface.hpp"

namespace feather::utils
{
  class BufferFillerInterface;
}

namespace feather::printer
{
  class PrintingOrchestratorInterface;
}

namespace feather::windows
{
  class MainWindowInterface
  {
  public:
    static void setPrintingOrchestrator(std::shared_ptr<printer::PrintingOrchestratorInterface>);
    virtual std::unique_ptr<utils::windows::WindowInformationInterface> accept(std::shared_ptr<utils::windows::WindowInformationVisitorInterface>) = 0;
    virtual std::shared_ptr<utils::storage::AbstractStorage> const &getStorage() const = 0;
    virtual bool isSecondaryHexModeMainWindow() const;
    virtual void disableCursor();
    virtual void enableCursor();
    virtual void hide();
    virtual void showSecondaryWindowCursor(std::pair<pair, pair> const &, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>) = 0;
    virtual void moveCursor(feather::utils::Direction, std::shared_ptr<utils::windows::WindowInformationVisitorInterface>, std::optional<size_t>) = 0;
    virtual void print(utils::ScreenBuffer const &) = 0;
    virtual void refreshBuffer(feather::printer::PrintingVisitorInterface *) = 0;
    virtual void resizeWindow(size_t, size_t, size_t, size_t);
    virtual void setCursorPosition(pair);
    virtual void setCursorFromRealPosition(pair, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::unique_ptr<printer::PrintingVisitorInterface> &, bool = false) = 0;
    virtual void setFramePosition(pair) = 0;
    virtual void setState(utils::windows::WindowStateContainer const &) = 0;
    virtual void show();
    virtual WindowImplInterface const *getLibraryWindowHandler() const;
    virtual pair getCursorPosition() const;
    virtual pair getCursorRealPosition(printer::PrintingOrchestratorInterface *) const = 0;
    virtual pair getLastRealPosition() const = 0;
    virtual pair getWindowDimensions() const;
    virtual utils::windows::WindowStateContainer getState() const = 0;
    virtual std::unique_ptr<utils::BufferFillerInterface> const &getBufferFiller() const = 0;
    virtual utils::datatypes::Uuid const &getUUID(bool = false) const = 0;
    virtual utils::windows::FramePositions const &getFramePosition() const = 0;
    virtual ~MainWindowInterface();

  protected:
    MainWindowInterface();
    MainWindowInterface(WindowImplInterface *);
    static std::weak_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorWeak;
    std::unique_ptr<WindowImplInterface> nCursesLibraryWindow;
    bool isSecondaryHexModeWindow;
  };
} // namespace feather::windows
