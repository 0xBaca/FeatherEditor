#pragma once

#include "action/output/SearchSubstringActionOutput.hpp"
#include "utils/BufferFillerInterface.hpp"
#include "utils/algorithm/SearchEngine.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
    struct SearchSubstringActionInput
    {
        utils::Direction searchDirection;
        std::shared_ptr<utils::algorithm::SearchEngine> searchEngine;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitor;
        std::unique_ptr<printer::PrintingVisitorInterface> &halfFrameUpVisitor;
        std::unique_ptr<windows::SubWindowInterface> &currBottomBarWindow;
        utils::FEATHER_MODE &currentFeatherMode;
        SearchSubstringActionInput(utils::datatypes::Uuid const &, std::u32string &&, std::shared_ptr<utils::storage::AbstractStorageFactory>, std::shared_ptr<utils::storage::AbstractStorageFactory>, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::unique_ptr<utils::BufferFillerInterface> const &, std::shared_ptr<windows::WindowsManager> windowsManager, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<windows::SubWindowInterface> &, utils::FEATHER_MODE &, utils::Direction, pair, std::list<pair> &, std::list<pair>::iterator &, std::set<pair> &, bool);
    };
} // namespace feather::action