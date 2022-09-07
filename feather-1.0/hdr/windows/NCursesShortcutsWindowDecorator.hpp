#pragma once

#include "windows/SubWindowInterface.hpp"

namespace feather::windows
{
    class NCursesShortcutsWindowDecorator : public SubWindowInterface
    {
    public:
        explicit NCursesShortcutsWindowDecorator(std::shared_ptr<MainWindowInterface>, WindowImplInterface *);
        std::vector<std::u32string> const &getShortcuts() const;
        size_t getFirstLineNumber() const;
        void decrementFirstLineNumber();
        void incrementFirstLineNumber();
        void resetFirstLineNumber();
        void print(utils::ScreenBuffer const &) override;
        void refreshBuffer(printer::PrintingVisitorInterface *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
        ~NCursesShortcutsWindowDecorator();

    private:
        static std::vector<std::u32string> readModeShortcuts;
        static std::vector<std::u32string> writeModeShortcuts;
        size_t firstLineNumber;
    };
} // namespace feather::windows
