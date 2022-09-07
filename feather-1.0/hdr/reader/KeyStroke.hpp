#pragma once

#include "action/ActionHandler.hpp"
#include "printer/ChangesStack.hpp"
#include "utils/NCursesWrapper.hpp"
#include "utils/datatypes/Command.hpp"
#include "utils/datatypes/DeletionsSnapshot.hpp"
#include "utils/helpers/KeyReader.hpp"
#include "utils/helpers/RawCharactersBuffer.hpp"
#include "utils/storage/AbstractStorageFactory.hpp"
#include "windows/WindowsManager.hpp"

#include <atomic>

namespace feather::reader
{
  class KeyStroke
  {
  public:
    static void normalizeAfterSave(std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &);
    static void updateBreakPoints(pair, size_t, utils::datatypes::Uuid const &);
    static void cleanBreakPoints(pair, utils::datatypes::Uuid const &);
    KeyStroke(std::shared_ptr<windows::WindowsManager>, std::shared_ptr<utils::logger::policy::FileLogPolicy>, std::shared_ptr<utils::storage::AbstractStorageFactory>);
    void defaultRedrawWindows(std::shared_ptr<windows::MainWindowInterface>);
    void operator()();
    ~KeyStroke();

  private:
    std::chrono::time_point<std::chrono::steady_clock> lastAddBreakpointPressed;
    std::chrono::time_point<std::chrono::steady_clock> lastUndoRedoPressed;
    std::chrono::time_point<std::chrono::steady_clock> trialVersionTimeout;
    std::chrono::nanoseconds timestamp;
    std::atomic<bool> closeApplication;
    std::shared_ptr<action::ActionHandler> actionHandler;
    std::shared_ptr<printer::PrintingStorageInterface> filePrintingStorage, memoryPrintingStorage;
    std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
    std::shared_ptr<utils::Filesystem> filesystem;
    std::shared_ptr<utils::helpers::KeyReader> keyReader;
    std::shared_ptr<utils::logger::policy::FileLogPolicy> log;
    std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory;
    std::shared_ptr<windows::WindowsManager> windowsManager;
    std::shared_ptr<utils::windows::WindowInformationVisitorInterface> linesLengthInformationVisitor;
    std::unique_ptr<printer::PrintingVisitorInterface> currFrameVisitor, lineDownVisitor, lineUpVisitor, frameDownVisitor, frameUpVisitor, halfFrameDownVisitor, halfFrameUpVisitor;
    std::shared_ptr<printer::ChangesStack> changesStack;
    utils::helpers::RawCharactersBuffer inputBuffer;
    std::optional<std::pair<pair, pair>> selectedText;
    std::optional<std::pair<pair, pair>> emptySpacesToDiscard;
    static std::unordered_map<utils::datatypes::Uuid, std::list<pair>, utils::datatypes::UuidHasher> breakPoints;
    static std::unordered_map<utils::datatypes::Uuid, std::set<pair>, utils::datatypes::UuidHasher> breakPointsAdded;
    static std::unordered_map<utils::datatypes::Uuid, std::list<pair>::iterator, utils::datatypes::UuidHasher> currentBreakPoint;
    static std::unordered_map<utils::datatypes::Uuid, utils::datatypes::Command, utils::datatypes::UuidHasher> lastCommand;
    std::pair<action::ACTION_TYPE, size_t> actionType;
    void addBreakpoint(std::shared_ptr<windows::MainWindowInterface>, std::unique_ptr<windows::SubWindowInterface> &);
    void highlightTextIfNeeded(std::shared_ptr<windows::MainWindowInterface>);
    void nextBreakpoint(std::shared_ptr<windows::MainWindowInterface>, std::unique_ptr<windows::SubWindowInterface> &);
    void previousBreakpoint(std::shared_ptr<windows::MainWindowInterface>, std::unique_ptr<windows::SubWindowInterface> &);
    bool isCursorAtFirstLine() const;
    bool isCursorAtLastLine() const;
    bool isCursorAtLowerRightCorner() const;
    bool isCursorAtTopLeftCorner() const;
    bool isLastValidPosInFrame() const;
    bool shouldFrameBeRewindedBeforeDelete(pair) const;
    std::pair<action::ACTION_TYPE, size_t> waitForSecondKey(std::shared_ptr<windows::MainWindowInterface>, std::unique_ptr<windows::SubWindowInterface> &, utils::datatypes::ChangeType);
    void addNewCharacter(char32_t, std::shared_ptr<windows::MainWindowInterface>, std::unique_ptr<windows::SubWindowInterface> &);
    void backspaceHandler(std::shared_ptr<windows::MainWindowInterface>);
    void moveCursorLeft(std::shared_ptr<windows::MainWindowInterface>);
    void moveCursorRight(std::shared_ptr<windows::MainWindowInterface>);
    void refreshInfoWindow(std::unique_ptr<printer::PrintingVisitorInterface> &);
    void refreshProgressWindow();
    void refreshShortcutsWindow(std::unique_ptr<printer::PrintingVisitorInterface> &);
    void refresh(std::shared_ptr<windows::MainWindowInterface>, bool, std::optional<pair>);
    void rewindFrameIfNeeded(pair, std::shared_ptr<windows::MainWindowInterface>);
    void setModeToReadMode(std::unique_ptr<windows::SubWindowInterface> &);
    void setModeToWriteMode(std::unique_ptr<windows::SubWindowInterface> &);
    void unhiglightSelectedText(std::shared_ptr<windows::MainWindowInterface>);
    void unselectTextToCopy();
    void updateCursorPosIfFileModified(std::shared_ptr<windows::MainWindowInterface>, pair &, pair &);
    bool yesNoPrompt(std::shared_ptr<windows::MainWindowInterface>);
  };
} // namespace feather::reader