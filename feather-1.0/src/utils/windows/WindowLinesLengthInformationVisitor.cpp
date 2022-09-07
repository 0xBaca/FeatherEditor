#include "utils/windows/LinesAroundPositionInformation.hpp"
#include "utils/windows/WindowLinesLengthInformationVisitor.hpp"

namespace feather::utils::windows
{
    WindowLinesLengthInformationVisitor::WindowLinesLengthInformationVisitor()
    {
    }
    std::unique_ptr<WindowInformationInterface> WindowLinesLengthInformationVisitor::visit(feather::windows::NCursesMainWindowDecorator *window)
    {
        auto cursorPosition = window->getCursorPosition();

        auto const &windowBuffer = window->getBufferFiller()->getScreenBuffer();
        if (windowBuffer.isBufferEmpty())
        {
            return std::make_unique<LinesAroundPositionInformation>(std::make_tuple(std::make_pair(0, false), std::make_pair(0, false), std::make_pair(0, false)));
        }
        size_t lineUpLen = cursorPosition.first ? windowBuffer[cursorPosition.first - 1].size() : 0;
        bool isLineUpNewLineCharacter = lineUpLen ? (windowBuffer[cursorPosition.first - 1].back() == '\n') : false;
        size_t lineCurrLen = windowBuffer[cursorPosition.first].size();
        bool isCurrNewLineCharacter = lineCurrLen ? (windowBuffer[cursorPosition.first].back() == '\n') : false;

        size_t lineDownLen = getLineDownLen(cursorPosition.first, window, windowBuffer);
        bool isLineDownNewLineCharacter = lineDownLen ? (windowBuffer[cursorPosition.first + 1].back() == '\n') : false;

        return std::make_unique<LinesAroundPositionInformation>(std::make_tuple(
            std::make_pair(lineUpLen, isLineUpNewLineCharacter),
            std::make_pair(lineCurrLen, isCurrNewLineCharacter),
            std::make_pair(lineDownLen, isLineDownNewLineCharacter)));
    }
    std::unique_ptr<WindowInformationInterface> WindowLinesLengthInformationVisitor::visit(feather::windows::NCursesBottomBarWindowDecorator *window)
    {
        return std::unique_ptr<WindowInformationInterface>();
    }

    std::unique_ptr<WindowInformationInterface> WindowLinesLengthInformationVisitor::visit(feather::windows::NCursesProgressWindowDecorator *window)
    {
        return std::unique_ptr<WindowInformationInterface>();
    }

} // namespace feather::utils::windows
