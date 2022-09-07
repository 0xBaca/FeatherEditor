#pragma once

#include "printer/PrintingStorageInterface.hpp"

#include <gmock/gmock.h>

using namespace feather::printer;

namespace testing::mocks
{
    class PrintingStorageMock : public PrintingStorageInterface
    {
    public:
        MOCK_METHOD2(dumpDeletions, void(feather::utils::datatypes::Uuid const &, std::map<feather::pair, feather::pair> const &));
        MOCK_METHOD2(removeChanges, void(feather::utils::datatypes::Uuid const &, size_t));
        MOCK_METHOD3(dumpChanges, void(feather::utils::datatypes::Uuid const &, feather::pair, std::vector<char> const &));
        MOCK_METHOD4(addToNewChanges, void(feather::utils::datatypes::Uuid const &, feather::pair, std::vector<char> const &, size_t));
        MOCK_METHOD4(dumpColors, void(feather::utils::datatypes::Uuid const &, std::multimap<std::pair<feather::pair, feather::pair>, std::pair<size_t, std::set<feather::utils::datatypes::COLOR>>> const &, std::unordered_multimap<std::u32string, std::pair<size_t, std::set<feather::utils::datatypes::COLOR>>> const &, size_t));
        MOCK_METHOD4(getNewChanges, std::vector<char>(feather::utils::datatypes::Uuid const &, size_t, size_t, size_t));
    };
} // namespace testing::mocks
