#pragma once

#include "utils/datatypes/Colors.hpp"
#include "utils/datatypes/DeserializedChangesArchive.hpp"
#include "utils/datatypes/DeserializedColorsArchive.hpp"
#include "utils/datatypes/DeserializedDeletionsArchive.hpp"
#include "utils/datatypes/Global.hpp"
#include "utils/storage/AbstractStorage.hpp"

#include <map>
#include <set>
#include <vector>

namespace feather::utils
{
    class Serializer
    {
    public:
        static void serialize(size_t pos, std::vector<char> const &, std::shared_ptr<storage::AbstractStorage>);
        static void serialize(std::map<pair, pair> const &, std::shared_ptr<storage::AbstractStorage>);
        static void serialize(std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &, std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &, size_t, std::shared_ptr<storage::AbstractStorage>);
        static datatypes::DeserializedChangesArchive deserializeChange(std::shared_ptr<storage::AbstractStorage>);
        static datatypes::DeserializedColorsArchive deserializeColors(std::shared_ptr<storage::AbstractStorage>);
        static datatypes::DeserializedDeletionsArchive deserializeDeletion(std::shared_ptr<storage::AbstractStorage>);
    };
}