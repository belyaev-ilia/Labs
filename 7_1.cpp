#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <stdexcept>

class Entity {
protected:
    std::string name;
    int HP;
    int level;
public:
    Entity(const std::string& n, int h, int l)
        : name(n), HP(h), level(l) {}

    std::string getName() const { return name; }
    int getHP() const { return HP; }
    int getLevel() const { return level; }
    virtual void displayInfo() const = 0;
    virtual ~Entity() = default;
};

class Hero : public Entity {
public:
    Hero(const std::string& name, int HP, int level)
        : Entity(name, HP, level) {}

    void displayInfo() const override {
        std::cout << "Name: " << Entity::name << ", HP: " << Entity::HP << ", Level: " << Entity::level << '\n';
    }
};

template <typename T>
class ListEntities {
    std::vector<T> entities;
public:
    void addEntity(T entity) {
        entities.push_back(entity);
    }

    void display() const {
        for (const auto& entity : entities) {
            entity->displayInfo();
        }
    }

    const std::vector<T>& getEntities() const {
        return entities;
    }
};

void save(const ListEntities<std::shared_ptr<Entity>>& list, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing.");
    }

    for (const auto& entity : list.getEntities()) {
        file << entity->getName() << " " << entity->getHP() << " " << entity->getLevel() << "\n";
    }
}

void load(ListEntities<std::shared_ptr<Entity>>& list, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading.");
    }

    std::string name;
    int HP, level;

    while (file >> name >> HP >> level) {
        list.addEntity(std::make_shared<Hero>(name, HP, level));
    }
}

int main() {
    try {
        ListEntities<std::shared_ptr<Entity>> LiEn;
        LiEn.addEntity(std::make_shared<Hero>("Knight", 150, 5));
        LiEn.addEntity(std::make_shared<Hero>("Goblin", 20, 1));
        LiEn.addEntity(std::make_shared<Hero>("Dragon", 500, 12));
        save(LiEn, "7_1.game_save.txt");

        ListEntities<std::shared_ptr<Entity>> loadedList;
        load(loadedList, "7_1.game_save.txt");

        std::cout << "List of entities:\n";
        loadedList.display();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

