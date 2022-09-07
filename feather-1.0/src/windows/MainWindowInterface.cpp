#include <utils/helpers/Lambda.hpp>
#include <windows/MainWindowInterface.hpp>

namespace feather::windows
{
    MainWindowInterface::MainWindowInterface()
    {
    }

    MainWindowInterface::MainWindowInterface(WindowImplInterface *nCursesLibraryWindowArg) : nCursesLibraryWindow(nCursesLibraryWindowArg)
    {
    }

    pair MainWindowInterface::getCursorPosition() const
    {
        return nCursesLibraryWindow->getCursorPosition();
    }

    pair MainWindowInterface::getWindowDimensions() const
    {
        return nCursesLibraryWindow->getWindowDimensions();
    }

    WindowImplInterface const *MainWindowInterface::getLibraryWindowHandler() const
    {
        return nCursesLibraryWindow.get();
    }

    bool MainWindowInterface::isSecondaryHexModeMainWindow() const
    {
        return isSecondaryHexModeWindow;
    }

    void MainWindowInterface::enableCursor()
    {
        nCursesLibraryWindow->enableCursor();
    }

    void MainWindowInterface::disableCursor()
    {
        nCursesLibraryWindow->disableCursor();
    }

    void MainWindowInterface::hide()
    {
        nCursesLibraryWindow->hide();
    }

    void MainWindowInterface::resizeWindow(size_t newWindowStartRow, size_t newWindowStartColumn, size_t newWindowNoRows, size_t newWindowNoColumns)
    {
        nCursesLibraryWindow->resizeWindow(newWindowStartRow, newWindowStartColumn, newWindowNoRows, newWindowNoColumns);
    }

    void MainWindowInterface::setCursorPosition(pair pos)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
        nCursesLibraryWindow->setCursorAtPos(pos);
    }

    void MainWindowInterface::setPrintingOrchestrator(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator)
    {
        printingOrchestratorWeak = printingOrchestrator;
    }

    void MainWindowInterface::show()
    {
        nCursesLibraryWindow->show();
    }

    MainWindowInterface::~MainWindowInterface()
    {
    }

    std::weak_ptr<printer::PrintingOrchestratorInterface> MainWindowInterface::printingOrchestratorWeak;
}