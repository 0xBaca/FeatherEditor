#pragma once

#include "utils/datatypes/Uuid.hpp"

#include <functional>

namespace feather::utils::datatypes
{
struct UuidHasher
{
    std::size_t operator()(const Uuid &uuid) const
    {
        std::hash<std::string> stringHasher;
        return stringHasher(uuid.getString());
    }
};
}
