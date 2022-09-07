#include "utils/helpers/Lambda.hpp"
#include "utils/storage/AbstractStorage.hpp"

namespace feather::utils::storage
{
    bool AbstractStorage::isCurrCharacterNewLine()
    {
        return utils::helpers::Lambda::isNewLineChar(getCurrentCharacter());
    }

    AbstractStorage::~AbstractStorage() = default;
} // namespace feather::utils::storage
