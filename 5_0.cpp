#include <iostream>
#include <vector>
#include <string>

template <typename T>
class Queue
{
private:
    std::vector<T> items;

public:
    void push(const T& item)
    {
        items.push_back(item);
    }

    void pop()
    {
        if (!items.empty()) {
            items.erase(items.begin());
        }
        else {
            std::cout << "Queue is empty. Nothing to pop.\n";
        }
    }

    void displayInfo() const
    {
        for (const auto& item : items)
        {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
};

int main()
{
    std::cout << "Queue with strings:\n";
    Queue<std::string> stringQueue;
    stringQueue.push("Sword");
    stringQueue.push("Sheld");
    stringQueue.push("Bow");
    std::cout << "Initial queue: ";
    stringQueue.displayInfo();
    stringQueue.pop();
    std::cout << "After popped the first elemtn: ";
    stringQueue.displayInfo();

    std::cout << "Queue with integers:\n";
    Queue<int> intQueue;
    intQueue.push(25);
    intQueue.push(50);
    intQueue.push(100);
    std::cout << "Initial queue: ";
    intQueue.displayInfo();
    intQueue.pop();
    std::cout << "After popped the first elemtn: ";
    intQueue.displayInfo();
    return 0;
}