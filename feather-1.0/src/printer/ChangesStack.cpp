#include "config/Config.hpp"
#include "printer/ChangesStack.hpp"
#include "utils/datatypes/Strings.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::printer
{
    void ChangesStack::addChange(utils::datatypes::ChangeType changeType, utils::datatypes::Uuid const &windowUUID, pair undoPos, pair redoPos, std::map<pair, pair> range)
    {
        undoStack[windowUUID].emplace(utils::datatypes::ChangePOD(changeType, undoPos, redoPos, range));
    }

    void ChangesStack::addChange(utils::datatypes::ChangeType changeType, utils::datatypes::Uuid const &windowUUID, pair undoPos, pair redoPos, std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> selectedText)
    {
        undoStack[windowUUID].emplace(utils::datatypes::ChangePOD(changeType, undoPos, redoPos, selectedText));
    }

    void ChangesStack::appendInsert(size_t size, utils::datatypes::Uuid const &windowUUID)
    {
        auto change = undoStack[windowUUID].top();
        undoStack[windowUUID].pop();
        validateChanges(change.ranges.rbegin()->second, size, windowUUID);
        change.ranges.rbegin()->second.second += size;
        change.redoPosition.second += size;
        undoStack[windowUUID].push(change);
    }

    std::optional<utils::datatypes::ChangePOD> ChangesStack::undoChange(utils::datatypes::Uuid const &windowUUID)
    {
        if (!isUndoStackEmpty(windowUUID))
        {
            auto change = undoStack[windowUUID].top();
            undoStack[windowUUID].pop();
            redoStack[windowUUID].push(change);
            return change;
        }
        return std::nullopt;
    }

    std::optional<utils::datatypes::ChangePOD> ChangesStack::redoChange(utils::datatypes::Uuid const &windowUUID)
    {
        if (!isRedoStackEmpty(windowUUID))
        {
            auto change = redoStack[windowUUID].top();
            redoStack[windowUUID].pop();
            undoStack[windowUUID].push(change);
            return change;
        }
        return std::nullopt;
    }

    void ChangesStack::clearRedoChanges(utils::datatypes::Uuid const &windowUUID)
    {
        redoStack[windowUUID] = std::stack<utils::datatypes::ChangePOD>();
    }

    void ChangesStack::clearUndoChanges(utils::datatypes::Uuid const &windowUUID)
    {
        undoStack[windowUUID] = std::stack<utils::datatypes::ChangePOD>();
    }

    bool ChangesStack::isUndoStackEmpty(utils::datatypes::Uuid const &windowUUID)
    {
        return undoStack[windowUUID].empty();
    }

    bool ChangesStack::isRedoStackEmpty(utils::datatypes::Uuid const &windowUUID)
    {
        return redoStack[windowUUID].empty();
    }

    void ChangesStack::validateChanges(pair pos, size_t size, utils::datatypes::Uuid const &windowUUID)
    {
        std::stack<utils::datatypes::ChangePOD> tmpStack;
        //Undo stack
        while (!undoStack[windowUUID].empty())
        {
            utils::datatypes::ChangePOD change = undoStack[windowUUID].top();
            change.updatePositions(pos, size);
            tmpStack.push(change);
            undoStack[windowUUID].pop();
        }
        while (!tmpStack.empty())
        {
            utils::datatypes::ChangePOD change = tmpStack.top();
            undoStack[windowUUID].push(change);
            tmpStack.pop();
        }
    }

    void ChangesStack::updatePositionsAfterSave(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, utils::datatypes::Uuid const &windowUUID)
    {
        std::stack<utils::datatypes::ChangePOD> tmpStack;
        //Undo stack
        while (!undoStack[windowUUID].empty())
        {
            utils::datatypes::ChangePOD change = undoStack[windowUUID].top();
            change.updatePositionsAfterSave(printingOrchestrator, windowUUID);
            tmpStack.push(change);
            undoStack[windowUUID].pop();
        }
        while (!tmpStack.empty())
        {
            utils::datatypes::ChangePOD change = tmpStack.top();
            undoStack[windowUUID].push(change);
            tmpStack.pop();
        }
        //Redo stack
        while (!redoStack[windowUUID].empty())
        {
            utils::datatypes::ChangePOD change = redoStack[windowUUID].top();
            change.updatePositionsAfterSave(printingOrchestrator, windowUUID);
            redoStack[windowUUID].push(change);
            redoStack[windowUUID].pop();
        }
        while (!tmpStack.empty())
        {
            utils::datatypes::ChangePOD change = tmpStack.top();
            redoStack[windowUUID].push(change);
            tmpStack.pop();
        }
    }

    void ChangesStack::updatePositionsAfterFileReload(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, pair lastValidPos, utils::datatypes::Uuid const &windowUUID)
    {
        std::stack<utils::datatypes::ChangePOD> tmpStack;
        //Undo stack
        while (!undoStack[windowUUID].empty())
        {
            utils::datatypes::ChangePOD change = undoStack[windowUUID].top();
            undoStack[windowUUID].pop();
            if (change.undoPosition <= lastValidPos && change.redoPosition <= lastValidPos)
            {
                tmpStack.push(change);
            }
        }
        while (!tmpStack.empty())
        {
            utils::datatypes::ChangePOD change = tmpStack.top();
            undoStack[windowUUID].push(change);
            tmpStack.pop();
        }
        //Redo stack
        while (!redoStack[windowUUID].empty())
        {
            utils::datatypes::ChangePOD change = redoStack[windowUUID].top();
            redoStack[windowUUID].pop();
            if (change.undoPosition <= lastValidPos && change.redoPosition <= lastValidPos)
            {
                tmpStack.push(change);
            }
            redoStack[windowUUID].push(change);
        }
        while (!tmpStack.empty())
        {
            utils::datatypes::ChangePOD change = tmpStack.top();
            redoStack[windowUUID].push(change);
            tmpStack.pop();
        }
    }

    ChangesStack::~ChangesStack()
    {
    }
} // namespace feather::printer
