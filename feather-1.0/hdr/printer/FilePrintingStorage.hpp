#pragma once

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif
#include "printer/PrintingStorageInterface.hpp"
#include "utils/storage/AbstractStorageFactory.hpp"

namespace feather::printer
{
  class FilePrintingStorage : public PrintingStorageInterface
  {
#ifdef _FEATHER_TEST_
    FRIEND_TEST(FilePrintingStorageTest, addCharacterToDeletions);
    FRIEND_TEST(FilePrintingStorageTest, addCharacterToDummyDeletions);
    FRIEND_TEST(FilePrintingStorageTest, addCharacterToChanges);
    FRIEND_TEST(FilePrintingStorageTest, addCharacterToChangesWhenPresent);
    FRIEND_TEST(FilePrintingStorageTest, addCharacterToChangesWhenPresent_2);
    FRIEND_TEST(FilePrintingStorageTest, dumpCacheChanges);
    FRIEND_TEST(FilePrintingStorageTest, dumpCacheDeletions);
    FRIEND_TEST(FilePrintingStorageTest, dumpCacheDummyDeletions);
    FRIEND_TEST(FilePrintingStorageTest, getChangesFile);
    FRIEND_TEST(FilePrintingStorageTest, mergeDeletions);
    FRIEND_TEST(FilePrintingStorageTest, mergeNewChanges);
    FRIEND_TEST(FilePrintingStorageTest, mergeNewChangesToLeft);
    FRIEND_TEST(FilePrintingStorageTest, mergeNewChangesToRight);
    FRIEND_TEST(FilePrintingStorageTest, mergeDummyDeletions);
    FRIEND_TEST(FilePrintingStorageTest, removeCharacterFromChanges);
#endif
  private:
    std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory;
    std::pair<bool, std::shared_ptr<utils::storage::AbstractStorage>> getChangesFile(size_t, std::string &&, utils::datatypes::Uuid const &);
    std::shared_ptr<utils::storage::AbstractStorage> getStorage(std::string &&, utils::datatypes::Uuid const &);
    std::string getChangesPath(utils::datatypes::Uuid const &, size_t, std::string const = "");
    std::string getDeletionsPath(utils::datatypes::Uuid const &, std::string const &);

  public:
    FilePrintingStorage(std::shared_ptr<utils::storage::AbstractStorageFactory>);
    void addToNewChanges(utils::datatypes::Uuid const &, pair, std::vector<char> const &, size_t) override;
    std::vector<char> getNewChanges(utils::datatypes::Uuid const &, size_t, size_t, size_t) override;
    void removeChanges(utils::datatypes::Uuid const &, size_t) override;
    void dumpChanges(utils::datatypes::Uuid const &, pair, std::vector<char> const &) override;
    void dumpColors(utils::datatypes::Uuid const &, std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &, std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &, size_t) override;
    void dumpDeletions(utils::datatypes::Uuid const &, std::map<pair, pair> const &) override;
  };
} // namespace feather::printer
