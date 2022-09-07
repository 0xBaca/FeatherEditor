#include "utils/datatypes/Uuid.hpp"

namespace feather::utils::datatypes
{

    Uuid::Uuid(std::string uuidArg)
    {
        uuid = uuidArg;
    }

    Uuid::Uuid(unsigned int length)
    {
        uuid = generate_hex(length);
    }

    std::string const & Uuid::getString() const
    {
        return uuid;
    }
} // namespace feather::utils::datatypes
