//
// Created by childcity on 05.05.20.
//

#ifndef LEARNMULTITHREADC_THREADSAFELIST_HPP
#define LEARNMULTITHREADC_THREADSAFELIST_HPP


#include <mutex>
#include <memory>
#include <functional>


namespace childcity {
namespace threadsafelist {

/*
    List structure
    Head{null; next->Node1} => Node1{data1; next->Node2} => Node2{data2; next->Node3} => ... NodeN{dataN; null}
*/

template<class T>
struct ThreadSafeList {
private:
    // Node of list
    struct Node {
        mutable std::mutex Mutex;
        std::shared_ptr<T> Data;
        std::unique_ptr<Node> Next;
        Node()
            : Next()
        {}
        Node(const T &value)
            : Data(std::make_shared<T>(value))
        {}
    };

private:
    Node head_;

public:
    ThreadSafeList()
    {}

    ThreadSafeList(const ThreadSafeList &other) = delete;
    ThreadSafeList &operator=(const ThreadSafeList &other) = delete;

    ~ThreadSafeList()
    {
        removeAll();
    }

    void pushFront(T newVal)
    {
        std::unique_ptr<Node> newHead = std::make_unique<Node>(std::forward<T>(newVal));
        std::lock_guard<std::mutex> lock(head_.Mutex);
        newHead->Next = std::move(head_.Next);
        head_.Next = std::move(newHead);
    }

    template <typename Functor>
    void forEach(Functor action)
    {
        forEach_([&](const Node *next) {
            action(*next->Data);
            return true; // means, we should continue to iterate
        });
    }

    template <typename Functor>
    std::shared_ptr<T> findFirstIf(Functor predicate)
    {
        std::shared_ptr<T> result;

        forEach_([&](const Node *next) mutable {
            if (predicate(*next->Data)) {
                result = next->Data;
                return false; // means, we should stop
            }
            return true; // means, we should continue to iterate
        });

        return result;
    }

    template <typename Functor>
    void removeIf(Functor predicate)
    {
        Node *current = &head_;
        Node *next = nullptr;

        std::unique_lock<std::mutex> lock(current->Mutex);
        while ((next = current->Next.get()) != nullptr) {
            std::unique_lock<std::mutex> lockNext(next->Mutex);

            if (predicate(*next->Data)) {
                std::unique_ptr<Node> old = std::move(current->Next);
                current->Next = std::move(old->Next);
                lockNext.unlock();
            } else {
                lock.unlock();
                current = next;
                lock = std::move(lockNext);
            }
        }
    }

    void removeAll()
    {
        removeIf([=](auto) { return true; });
    }

    std::shared_ptr<T> at(size_t index)
    {
        std::shared_ptr<T> result;
        size_t currentIt = 0;

        forEach_([&](const Node *next) mutable {
            if (currentIt++ == index) {
                result = next->Data;
                return false; // means, we should stop
            }
            return true; // means, we should continue to iterate
        });

        return result;
    }

private:
    template <typename Functor>
    void forEach_(Functor action)
    {
        const Node *current = &head_;
        const Node *next = nullptr;

        std::unique_lock<std::mutex> lock(current->Mutex);
        while ((next = current->Next.get()) != nullptr) {
            std::unique_lock<std::mutex> lockNext(next->Mutex);
            lock.unlock();

            if (! action(next)) {
                return;
            }

            current = next;
            lock = std::move(lockNext);
        }
    }
};


} // namespace threadsafelist
} // namespace childcity

#endif //LEARNMULTITHREADC_THREADSAFELIST_HPP