#pragma once

namespace feather::utils::datatypes
{
    enum class ChangeType
    {
        COPY,
        CUT,
        HIGHLIGHT,
        INSERT,
        DELETE,
        OTHER,
        UNKNOWN
    };
}