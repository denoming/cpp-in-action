// Copyright 2025 Denys Asauliak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <atomic>
#include <concepts>
#include <thread>
#include <future>

using namespace testing;

// clang-format off
template<typename T>
concept Nodable = requires(T n) {
    { T::data };
    { *n.next } -> std::same_as<T&>;
};
// clang-format on

template<typename T>
struct ListNode {
    T data;
    ListNode* next{nullptr};

    explicit ListNode(T d)
        : data{std::move(d)}
    {
    }
};

template<typename T, Nodable Node = ListNode<T>>
struct HazardPointer {
    std::atomic<std::thread::id> id;
    std::atomic<Node*> node;

    [[nodiscard]] bool
    tryPlace(std::thread::id newId)
    {
        std::thread::id oldId;
        return id.compare_exchange_strong(oldId, newId);
    }
};

/* Max amount of hazard pointers */
constexpr std::size_t MaxHazardPointers = 50;

/* Available hazard pointers */
template<typename T>
static HazardPointer<T> HazardPointers[MaxHazardPointers];

/**
 * The hazard pointer owner (the owner is a thread)
 */
template<typename T, Nodable Node = ListNode<T>>
class HazardPointerOwner {
public:
    HazardPointerOwner(HazardPointerOwner const&) = delete;
    HazardPointerOwner
    operator=(HazardPointerOwner const&)
        = delete;

    HazardPointerOwner()
    {
        for (std::size_t n{0}; n < MaxHazardPointers; ++n) {
            std::thread::id id;
            if (HazardPointers<T>[n].tryPlace(std::this_thread::get_id())) {
                _ptr = &HazardPointers<T>[n];
                break;
            }
        }
        if (_ptr == nullptr) {
            throw std::out_of_range{"No available hazard pointer"};
        }
    }

    ~HazardPointerOwner()
    {
        _ptr->id = std::thread::id{};
        _ptr->node = nullptr;
    }

    std::atomic<Node*>&
    node()
    {
        return _ptr->node;
    }

private:
    HazardPointer<T>* _ptr{nullptr};
};

/**
 * The list of nodes that are going to freed
 */
template<typename T, Nodable Node = ListNode<T>>
class RetireList {
public:
    void
    add(Node* n)
    {
        auto* e = new Entry{n, _head.load()};
        while (!_head.compare_exchange_strong(e->next, e)) { };
    }

    [[nodiscard]] bool
    used(Node* node) const
    {
        for (std::size_t n{0}; n < MaxHazardPointers; ++n) {
            if (HazardPointers<T>[n].node == node) {
                return true;
            }
        }
        return false;
    }

    void
    deleteUnused()
    {
        Entry* current = _head.exchange(nullptr);
        while (current) {
            Entry* next = current->next;
            if (used(current->node)) {
                push(current);
            } else {
                delete current;
            }
            current = next;
        }
    }

private:
    struct Entry {
        Node* node;
        Entry* next{nullptr};

        explicit Entry(Node* node, Entry* next)
            : node{node}
            , next{next}
        {
        }

        ~Entry()
        {
            delete node;
        }
    };

private:
    void
    push(Entry* entry)
    {
        entry->next = _head.load();
        while (!_head.compare_exchange_strong(entry->next, entry)) { };
    }

private:
    std::atomic<Entry*> _head;
};

/**
 * Returns thread local hazard pointer owner
 */
template<typename T, Nodable Node = ListNode<T>>
std::atomic<Node*>&
getHazardPointer()
{
    thread_local static HazardPointerOwner<T> owner;
    return owner.node();
}

/**
 * The lock free stack implementation
 */
template<typename T, Nodable Node = ListNode<T>>
class LockFreeStack {
public:
    LockFreeStack() = default;

    LockFreeStack(const LockFreeStack&) = delete;
    LockFreeStack&
    operator=(const LockFreeStack&)
        = delete;

    void
    push(T data)
    {
        Node* const newNode = new Node{std::move(data)};
        newNode->next = _head.load();
        while (!_head.compare_exchange_strong(newNode->next, newNode)) { };
    }

    T
    pop()
    {
        auto& hazardPtr = getHazardPointer<T>();
        Node* oldHead = _head.load();
        do {
            Node* node;
            do {
                node = oldHead;
                hazardPtr.store(oldHead);
                oldHead = _head.load();
            }
            while (node != oldHead);
        }
        while (oldHead && !_head.compare_exchange_strong(oldHead, oldHead->next));
        hazardPtr.store(nullptr);

        if (!oldHead) {
            throw std::out_of_range{"Stack is empty"};
        }
        auto data = oldHead->data;
        if (_retire.used(oldHead)) {
            _retire.add(oldHead);
        } else {
            delete oldHead;
        }
        _retire.deleteUnused();
        return data;
    }

private:
    std::atomic<Node*> _head;
    RetireList<T> _retire;
};

TEST(LockFreeStack, Dummy)
{
    LockFreeStack<int> stack;

    auto fut1 = std::async([&stack] { stack.push(2011); });
    auto fut2 = std::async([&stack] { stack.push(2012); });
    auto fut3 = std::async([&stack] { stack.push(2013); });

    auto fut4 = std::async([&stack] { return stack.pop(); });
    auto fut5 = std::async([&stack] { return stack.pop(); });
    auto fut6 = std::async([&stack] { return stack.pop(); });

    fut1.get(), fut2.get(), fut3.get();

    std::cout << fut4.get() << std::endl;
    std::cout << fut5.get() << std::endl;
    std::cout << fut6.get() << std::endl;
}