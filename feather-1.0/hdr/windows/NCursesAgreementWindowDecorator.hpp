#pragma once

#include "windows/MainWindowInterface.hpp"
#include "windows/SubWindowInterface.hpp"
#include "windows/WindowImplInterface.hpp"

namespace feather::windows
{
    class NCursesAgreementWindowDecorator : public SubWindowInterface
    {
    public:
        explicit NCursesAgreementWindowDecorator(std::shared_ptr<MainWindowInterface>, WindowImplInterface *);
        std::vector<std::u32string> const &getText() const;
        void print(utils::ScreenBuffer const &) override;
        void refreshBuffer(printer::PrintingVisitorInterface *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
        ~NCursesAgreementWindowDecorator();

    private:
        inline static const std::u32string REGISTERED_TO = U"Created by:";
        inline static const std::u32string FOR_HOST = U"";
        std::vector<std::u32string> lines;
    };
} // namespace feather::windows