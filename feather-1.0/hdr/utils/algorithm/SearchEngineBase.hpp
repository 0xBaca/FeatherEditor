#pragma once

#include "utils/BufferFiller.hpp"
#include "utils/algorithm/SearchBuffer.hpp"
#include "utils/algorithm/SearchEngine.hpp"

namespace feather::utils::algorithm
{
    class SearchEngineBase : public SearchEngine
    {
    public:
        static constexpr size_t MAX_SEARCH_RESULT_DELTA = 100;

    protected:
        static std::unordered_map<std::string, std::u32string> searchedStringWithoutLastChar;
        static std::pair<pair, size_t> getNextSearchInterval(std::pair<pair, pair>, std::string, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorage>, utils::datatypes::Uuid const &);
        SearchEngineBase(utils::datatypes::Uuid const &, std::u32string &&, std::shared_ptr<utils::storage::AbstractStorageFactory>, std::shared_ptr<utils::storage::AbstractStorageFactory>, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::unique_ptr<utils::BufferFillerInterface> const &, std::list<pair> &, std::list<pair>::iterator &, std::set<pair> &);
        void addBreakpoint(std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &, std::shared_ptr<feather::windows::WindowsManager> &);
        utils::datatypes::Uuid const &windowUUID;
        std::chrono::time_point<std::chrono::steady_clock> lastAddBreakpointPressed;
        std::u32string searchedString;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<utils::storage::AbstractStorage> toCursorStorage;
        std::shared_ptr<utils::storage::AbstractStorage> fromCursorStorage;
        std::shared_ptr<utils::storage::AbstractStorage> secondaryCursorStorage;
        std::shared_ptr<utils::storage::AbstractStorageFactory> fileStorageFactory;
        std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory;
        std::unique_ptr<printer::PrintingVisitorInterface> currentFrameVisitor;
        std::unique_ptr<utils::BufferFillerInterface> const &bufferFiller;
        size_t lastPos;
        std::pair<std::optional<std::pair<pair, size_t>>, utils::datatypes::ERROR> getNextMatch(std::shared_ptr<feather::windows::WindowsManager>, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<feather::windows::SubWindowInterface> &, utils::FEATHER_MODE &, utils::Direction) override;
        std::list<pair> &breakPoints;
        std::list<pair>::iterator &currentBreakPoint;
        std::set<pair> &breakPointsAdded;
        ~SearchEngineBase();
    };
} // namespace feather::utils::algorithm