#pragma once

#include "windows/MainWindowInterface.hpp"
#include "windows/SubWindowInterface.hpp"
#include "windows/WindowImplInterface.hpp"

namespace feather::windows
{
    class NCursesExitWindowDecorator : public SubWindowInterface
    {
    public:
        static std::u32string CHANGES_HAS_BEEN_MADE;
        static std::u32string NO_CHANGES_HAS_BEEN_MADE;
        explicit NCursesExitWindowDecorator(std::shared_ptr<MainWindowInterface>, WindowImplInterface *);
        std::vector<std::u32string> const &getText() const;
        void print(utils::ScreenBuffer const &) override;
        void refreshBuffer(printer::PrintingVisitorInterface *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
        ~NCursesExitWindowDecorator();

    private:
        static std::vector<std::u32string> text;
    };
} // namespace feather::windows
