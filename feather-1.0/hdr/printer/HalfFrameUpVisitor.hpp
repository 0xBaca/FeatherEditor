#pragma once

#include "printer/PrintingVisitorInterface.hpp"
#include "printer/WindowStatus.hpp"
#include "windows/NCursesAgreementWindowDecorator.hpp"
#include "windows/NCursesBottomBarWindowDecorator.hpp"
#include "windows/NCursesExitWindowDecorator.hpp"
#include "windows/NCursesInfoWindowDecorator.hpp"
#include "windows/NCursesMainWindowDecorator.hpp"
#include "windows/NCursesProgressWindowDecorator.hpp"
#include "windows/NCursesTopWindowDecorator.hpp"

namespace feather::printer
{
    class HalfFrameUpVisitor : public PrintingVisitorInterface, protected WindowStatus
    {
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;

    public:
        HalfFrameUpVisitor() = default;
        HalfFrameUpVisitor(std::shared_ptr<printer::PrintingOrchestratorInterface>);
        void visit(windows::NCursesMainWindowDecorator *) override;
        void visit(windows::NCursesAgreementWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
        void visit(windows::NCursesProgressWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
        void visit(windows::NCursesBottomBarWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
        void visit(windows::NCursesExitWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
        void visit(windows::NCursesInfoWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
        void visit(windows::NCursesShortcutsWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
        void visit(windows::NCursesTopWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
    };
} // namespace feather::printer