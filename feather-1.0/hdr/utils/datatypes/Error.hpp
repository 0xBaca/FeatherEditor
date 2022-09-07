#pragma once

namespace feather::utils::datatypes
{
    enum class ERROR
    {
        NONE,
        FILE_NO_LONGER_AVAILABLE,
        INTERRUPTED,
        ESC_INTERRUPTED,
        UNABLE_TO_OPEN_FILE
    };
}