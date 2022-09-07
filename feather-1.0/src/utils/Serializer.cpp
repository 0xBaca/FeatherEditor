#include "utils/Serializer.hpp"
#include "utils/datatypes/ChangesArchive.hpp"
#include "utils/datatypes/ColorsArchive.hpp"
#include "utils/datatypes/DeletionsArchive.hpp"

#include "cereal/types/set.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/archives/binary.hpp"

namespace cereal
{
    template <class Archive, class F, class S>
    void save(Archive &ar, const std::pair<F, S> &pair)
    {
        ar(pair.first, pair.second);
    }

    template <class Archive, class F, class S>
    void load(Archive &ar, std::pair<F, S> &pair)
    {
        ar(pair.first, pair.second);
    }

    template <class Archive, class F, class S>
    struct specialize<Archive, std::pair<F, S>, cereal::specialization::non_member_load_save>
    {
    };
}

namespace feather::utils
{
    void Serializer::serialize(size_t pos, std::vector<char> const &buffer, std::shared_ptr<storage::AbstractStorage> storage)
    {
        cereal::BinaryOutputArchive archive(storage->getRawStream());
        datatypes::ChangesArchive a{FEATHER_WATERMARK, pos, buffer};
        archive(a);
    }

    void Serializer::serialize(std::map<pair, pair> const &v, std::shared_ptr<storage::AbstractStorage> storage)
    {
        cereal::BinaryOutputArchive archive(storage->getRawStream());
        datatypes::DeletionsArchive a{FEATHER_WATERMARK, v};
        archive(a);
    }

    void Serializer::serialize(std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &textChunksColors, std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &textColors, size_t colorPriority, std::shared_ptr<storage::AbstractStorage> storage)
    {
        cereal::BinaryOutputArchive archive(storage->getRawStream());
        datatypes::ColorsArchive a{FEATHER_WATERMARK, colorPriority, textChunksColors, textColors};
        archive(a);
    }

    datatypes::DeserializedChangesArchive Serializer::deserializeChange(std::shared_ptr<storage::AbstractStorage> storage)
    {
        cereal::BinaryInputArchive archive(storage->getRawStream());
        datatypes::DeserializedChangesArchive a;
        archive(a);
        return a;
    }

    datatypes::DeserializedColorsArchive Serializer::deserializeColors(std::shared_ptr<storage::AbstractStorage> storage)
    {
        cereal::BinaryInputArchive archive(storage->getRawStream());
        datatypes::DeserializedColorsArchive a;
        archive(a);
        return a;
    }

    datatypes::DeserializedDeletionsArchive Serializer::deserializeDeletion(std::shared_ptr<storage::AbstractStorage> storage)
    {
        cereal::BinaryInputArchive archive(storage->getRawStream());
        datatypes::DeserializedDeletionsArchive a;
        archive(a);
        return a;
    }

}