#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/datatypes/DeletionsSnapshot.hpp"
#include "windows/WindowsManager.hpp"

#include <list>

namespace feather::action
{
    struct PasteActionInput
    {
        utils::datatypes::Uuid const &windowUUID;
        std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>> source;
        std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory;
        pair currCursorPos;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>> textCopyPositionsToSkip;
        std::list<pair> &breakPoints;
        std::list<pair>::iterator &currentBreakPoint;
        std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> deletionsSnapshot;
        std::set<pair> addedBreakPoints;
        std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitor;
        bool isPastingNewLine;
        PasteActionInput(utils::datatypes::Uuid const &, std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>, std::shared_ptr<utils::storage::AbstractStorageFactory>, pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>>, std::list<pair> &, std::list<pair>::iterator &, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>>, std::set<pair>, std::shared_ptr<windows::WindowsManager>, std::unique_ptr<printer::PrintingVisitorInterface> &, bool);
    };
} // namespace feather::action