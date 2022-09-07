#include "utils/datatypes/DeletionsSnapshot.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::utils::datatypes
{
    void DeletionsSnapshot::shift(pair pos, size_t size)
    {
        deletions = utils::helpers::Lambda::shiftIntervals(deletions, pos, size, std::nullopt);
        reverseDeletions.clear();
        for (auto &deletion : deletions)
        {
            reverseDeletions[deletion.second] = deletion.first;
        }
    }
}