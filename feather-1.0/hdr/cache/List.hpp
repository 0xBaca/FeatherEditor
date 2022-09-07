#pragma once

#include <memory>
#include <mutex>

#include "utils/datatypes/Uuid.hpp"

namespace feather::cache
{
class Cache;

class List
{
  friend class Cache;
  class Node
  {
    friend class List;

  public:
    explicit Node(utils::datatypes::Uuid const &, size_t, std::vector<char> const &, size_t);
    void addChanges(std::vector<char> const &, size_t, size_t);
    std::tuple<utils::datatypes::Uuid, size_t> getChangesInfo() const;
    std::vector<char> const &getElements() const;
    size_t getSize() const;
    ~Node();

  private:
    utils::datatypes::Uuid windowUUID;
    size_t changePosition;
    std::shared_ptr<Node> prev, next;
    std::vector<char> buffer;
    std::mutex nodeMutex;
  };

public:
  List();
  std::shared_ptr<typename List::Node> addNewNode(utils::datatypes::Uuid const &, size_t);
  std::shared_ptr<typename List::Node> getLeastRecentlyUsed() const;
  std::shared_ptr<typename List::Node> moveToBegining(std::shared_ptr<typename List::Node>);
  void removeTail();
  void removeNode(std::shared_ptr<typename List::Node>);
  void removeNodeLRU();
  bool isListEmpty() const;
  ~List();

private:
  void removeHead();
  std::shared_ptr<Node> head, tail;
  size_t noNodes;
};
} // namespace feather::cache
