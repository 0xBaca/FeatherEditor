#include "cache/List.hpp"
#include "utils/Utf8Util.hpp"

namespace feather::cache
{
    List::Node::Node(utils::datatypes::Uuid const &windowUUIDArg, size_t changePositionArg, std::vector<char> const &bufferArg, size_t bufferSize)
        : windowUUID(windowUUIDArg), changePosition(changePositionArg), prev(nullptr), next(nullptr), buffer(bufferArg.begin(), std::next(bufferArg.begin(), bufferSize))
    {
    }

    void List::Node::addChanges(std::vector<char> const &changes, size_t changesSize, size_t offset)
    {
        size_t oldBufferSize = buffer.size();
        buffer.resize(oldBufferSize + changesSize);
        std::copy_backward(std::next(buffer.begin(), offset), std::next(buffer.begin(), oldBufferSize), buffer.end());
        std::copy(changes.begin(), std::next(changes.begin(), changesSize), std::next(buffer.begin(), offset));
    }

    std::tuple<utils::datatypes::Uuid, size_t> List::Node::getChangesInfo() const
    {
        return std::make_tuple(windowUUID, changePosition);
    }

    std::vector<char> const &List::Node::getElements() const
    {
        return buffer;
    }

    size_t List::Node::getSize() const
    {
        return buffer.size();
    }

    List::Node::~Node()
    {
    }

    List::List() : head(nullptr), tail(nullptr), noNodes(0)
    {
    }

    std::shared_ptr<List::Node> List::addNewNode(utils::datatypes::Uuid const &windowUUID, size_t pos)
    {
        std::shared_ptr<Node> newHead = std::make_shared<Node>(windowUUID, pos, std::vector<char>{}, 0);
        if (isListEmpty())
        {
            ++noNodes;
            return head = tail = newHead;
        }
        newHead->next = head;
        head->prev = newHead;
        head = newHead;
        ++noNodes;
        return head;
    }

    std::shared_ptr<List::Node> List::getLeastRecentlyUsed() const
    {
        return tail;
    }

    std::shared_ptr<List::Node> List::moveToBegining(std::shared_ptr<List::Node> node)
    {
        std::unique_lock<std::mutex> lockHead(head->nodeMutex);
        std::unique_lock<std::mutex> lockNext, lockPrev;

        if (node.get() == head.get())
        {
            return head;
        }

        if (head != node->prev)
        {
            lockPrev = std::unique_lock<std::mutex>(node->prev->nodeMutex);
        }
        std::lock_guard<std::mutex> lockThis(node->nodeMutex);

        if (node->next)
        {
            lockNext = std::unique_lock<std::mutex>(node->next->nodeMutex);
            node->next->prev = node->prev;
        }

        node->prev->next = node->next;
        node->next = head;
        head->prev = node;
        head = node;
        return head;
    }

    void List::removeHead()
    {
        std::unique_lock<std::mutex> lockHead(head->nodeMutex);
        if (!isListEmpty())
        {
            head = head->next;
            if (head)
            {
                head->prev = nullptr;
            }
            --noNodes;
        }
    }

    void List::removeTail()
    {
        std::unique_lock<std::mutex> lockHead(head->nodeMutex);
        if (!isListEmpty())
        {
            if (head.get() == tail.get())
            {
                head.reset();
                tail.reset();
            }
            else
            {
                lockHead.unlock();
                std::lock(tail->prev->nodeMutex, tail->nodeMutex);
                std::lock_guard<std::mutex> lockTailPrev(tail->prev->nodeMutex, std::adopt_lock);
                std::lock_guard<std::mutex> lockTail(tail->nodeMutex, std::adopt_lock);
                tail = tail->prev;
                tail->next.reset();
            }
            --noNodes;
        }
    }

    bool List::isListEmpty() const
    {
        return !noNodes;
    }

    void List::removeNode(std::shared_ptr<typename List::Node> node)
    {
        moveToBegining(node);
        removeHead();
    }

    List::~List()
    {
    }
} // namespace feather::cache
