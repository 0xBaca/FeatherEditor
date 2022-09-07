#pragma once

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif

#include "utils/datatypes/ChangePOD.hpp"

#include <stack>

namespace feather::printer
{
    class ChangesStack
    {
#ifdef _FEATHER_TEST_
        friend class ChangesStackTest;
        FRIEND_TEST(ChangesStackTest, addChangeWithoutAppending);
        FRIEND_TEST(ChangesStackTest, addChangeWithAppending);
        FRIEND_TEST(ChangesStackTest, undoInsertsOnly);
        FRIEND_TEST(ChangesStackTest, redoInsertsOnly);
#endif
    private:
        std::map<utils::datatypes::Uuid, std::stack<utils::datatypes::ChangePOD>> undoStack, redoStack;

    public:
        void addChange(utils::datatypes::ChangeType, utils::datatypes::Uuid const &, pair, pair, std::map<pair, pair>);
        void addChange(utils::datatypes::ChangeType, utils::datatypes::Uuid const &, pair, pair, std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>>);
        void appendInsert(size_t, utils::datatypes::Uuid const &);
        std::optional<utils::datatypes::ChangePOD> undoChange(utils::datatypes::Uuid const &);
        std::optional<utils::datatypes::ChangePOD> redoChange(utils::datatypes::Uuid const &);
        void clearRedoChanges(utils::datatypes::Uuid const &);
        void clearUndoChanges(utils::datatypes::Uuid const &);
        bool isUndoStackEmpty(utils::datatypes::Uuid const &);
        bool isRedoStackEmpty(utils::datatypes::Uuid const &);
        void updatePositionsAfterSave(std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &);
        void updatePositionsAfterFileReload(std::shared_ptr<printer::PrintingOrchestratorInterface>, pair, utils::datatypes::Uuid const &);
        void validateChanges(pair, size_t, utils::datatypes::Uuid const &);
        ~ChangesStack();
    };
} // namespace feather::printer