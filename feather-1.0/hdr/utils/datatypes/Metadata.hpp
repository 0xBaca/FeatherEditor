#pragma once

#include "utils/datatypes/ChangeType.hpp"

#include <vector>

namespace feather::utils::datatypes
{
struct Metadata
{
  struct MetadataPOD
  {
    ChangeType changeType;
    size_t noChangesBytes;
    size_t changesStartPos;
    unsigned short firstChangeCharacterBytes;
    MetadataPOD(ChangeType);
    MetadataPOD(size_t, size_t, ChangeType);
    bool isInsertChange() const;
    MetadataPOD addChangesBytes(size_t);
    MetadataPOD addBytes(size_t);
    MetadataPOD addToEndPos(size_t);
    MetadataPOD removeChangesBytes(size_t);
    MetadataPOD setChangeType(ChangeType);
    MetadataPOD setStartPos(size_t);
    MetadataPOD setEndPos(size_t);
    MetadataPOD operator+(MetadataPOD const &&);
  };
  static MetadataPOD getHeaderFrom(size_t pos, std::vector<char> const &, ChangeType);
};
} // namespace feather::utils::datatypes
