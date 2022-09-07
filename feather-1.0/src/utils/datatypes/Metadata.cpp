#include "utils/datatypes/Global.hpp"
#include "utils/datatypes/Metadata.hpp"
#include "utils/Utf8Util.hpp"

namespace feather::utils::datatypes
{
    Metadata::MetadataPOD::MetadataPOD(ChangeType changeTypeArg)
        : changeType(changeTypeArg), noChangesBytes(0), changesStartPos(0)
    {
    }

    Metadata::MetadataPOD::MetadataPOD(size_t noChangesBytesArg, size_t changesStartPosArg, ChangeType changeTypeArg)
        : changeType(changeTypeArg), noChangesBytes(noChangesBytesArg), changesStartPos(changesStartPosArg)
    {
    }

    Metadata::MetadataPOD Metadata::MetadataPOD::addChangesBytes(size_t v)
    {
        noChangesBytes += v;
        return *this;
    }

    bool Metadata::MetadataPOD::isInsertChange() const
    {
        return (ChangeType::INSERT == changeType);
    }

    Metadata::MetadataPOD Metadata::MetadataPOD::removeChangesBytes(size_t v)
    {
        noChangesBytes -= v;
        return *this;
    }

    Metadata::MetadataPOD Metadata::MetadataPOD::setChangeType(ChangeType v)
    {
        changeType = v;
        return *this;
    }

    Metadata::MetadataPOD Metadata::MetadataPOD::setStartPos(size_t v)
    {
        changesStartPos = v;
        return *this;
    }

    Metadata::MetadataPOD Metadata::MetadataPOD::operator+(MetadataPOD const &&right)
    {
        return MetadataPOD{noChangesBytes, changesStartPos, changeType}
            .addChangesBytes(right.noChangesBytes);
    }

    Metadata::MetadataPOD Metadata::getHeaderFrom(size_t pos, std::vector<char> const &changes, ChangeType changeType)
    {
        return MetadataPOD{0, 0, changeType}
            .addChangesBytes(changes.size())
            .setStartPos(pos);
    }
} // namespace feather::utils::datatypes
