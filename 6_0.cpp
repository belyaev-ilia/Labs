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
        if (items.empty()) {
            throw std::invalid_argument("Queue is empty");
        }
        items.erase(items.begin());
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
    try
    {
        std::cout << "Empty queue:\n";
        Queue<std::string> emptyQueue;
        std::cout << "Initial queue: ";
        emptyQueue.displayInfo();
        emptyQueue.pop();
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Queue error: " << e.what() << '\n';
    }
    try
    {
        std::cout << "\nQueue with string:\n";
        Queue<std::string> stringQueue;
        stringQueue.push("Potion");
        std::cout << "Initial queue: ";
        stringQueue.displayInfo();
        stringQueue.pop();
        std::cout << "After popped the first elemtn: ";
        stringQueue.displayInfo();
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Queue error: " << e.what() << '\n';
    }

    return 0;
}