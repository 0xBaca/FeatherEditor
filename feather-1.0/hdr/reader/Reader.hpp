#pragma once

#include "action/ActionHandler.hpp"
#include "utils/NCursesWrapper.hpp"
#include "utils/helpers/KeyReader.hpp"
#include "utils/windows/FramePositions.hpp"
#include "windows/SubWindowInterface.hpp"

namespace feather::reader
{
    class Reader
    {
    public:
        Reader(std::unique_ptr<windows::SubWindowInterface> &, std::shared_ptr<action::ActionHandler>, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorageFactory>, std::shared_ptr<windows::WindowsManager>, std::shared_ptr<utils::helpers::KeyReader>, std::pair<utils::windows::FramePositions, pair>, std::list<pair> &, std::set<pair> &, std::list<pair>::iterator &);
        std::u32string getCommandBuffer() const;
        virtual bool isAtHistoryBegin() const;
        virtual bool isAtHistoryEnd() const;
        virtual std::optional<std::u32string> getCurrFromHistory() const;
        virtual std::optional<std::u32string> getNextFromHistory() const;
        virtual std::optional<std::u32string> getPreviousFromHistory() const;
        void trialEndedApplicationHandler();
        virtual ~Reader();

    protected:
        std::pair<utils::windows::FramePositions, pair> framePosition;
        bool anyChangeMade;
        size_t commandBufferFirstIdx;
        std::u32string commandBuffer;
        std::shared_ptr<action::ActionHandler> actionHandler;
        std::unique_ptr<printer::PrintingVisitorInterface> currFrameVisitor, halfFrameUpVisitor, lineDownVisitor, lineUpVisitor;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<utils::helpers::KeyReader> keyReader;
        std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory;
        std::unique_ptr<windows::SubWindowInterface> &currBottomBarWindow;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        std::list<pair> &breakPoints;
        std::set<pair> &breakPointsAdded;
        std::list<pair>::iterator &currentBreakPoint;

        void blockSignals();
        void unblockSignals();
        void print(std::u32string, bool = true);
        void printError(std::u32string);
        void printWithoutDelay(std::u32string, bool = true);
        void refreshBuffer(std::u32string);
        void setCursor(size_t);
        void setCursorAtTheEnd();
        void setCursorAtProperPosition(bool = true);
        void shiftCursorHalfWindowLeft(size_t);
        void terminalDimensionsChangeHandler(std::optional<std::u32string>, size_t);
        void terminalDimensionsChangeHandler(std::optional<std::u32string>, pair);
        bool isBufferEmpty() const;
        char32_t getNextCharacter(windows::WindowImplInterface const *);
        size_t removeWordBackward(size_t);
    };
} // namespace feather::reader
