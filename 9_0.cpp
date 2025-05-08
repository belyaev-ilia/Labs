#include <iostream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <array>

class Character;

template<typename T>
class Logger {
private:
    std::ofstream log_file;

    std::string getTimes() const {
        std::time_t now = std::time(nullptr);
        std::array<char, 26> time_buf;
        errno_t err = ctime_s(time_buf.data(), time_buf.size(), &now);

        if (err != 0) {
            std::cerr << "Error in ctime_s: " << err << std::endl;
            return "Error";
        }

        std::string time(time_buf.data());
        if (!time.empty() && time.back() == '\n') {
            time.pop_back();
        }
        return time;
    }

public:
    Logger(const std::string& filename) {
        log_file.open(filename, std::ios::app);
        if (!log_file.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filename);
        }
    }

    void log(const T& message) {
        if (!log_file.is_open()) {
            throw std::runtime_error("Log file is not open");
        }
        log_file << "[" << getTimes() << "] " << message << "\n";
        log_file.flush();
    }

    ~Logger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
};

class Item {
protected:
    std::string name;
public:
    Item(const std::string& n) : name(n) {}

    std::string getName() const { return name; }

    virtual void use(Character& hero) {};

    virtual ~Item() = default;
};

class Inventory {
private:
    std::vector<std::unique_ptr<Item>> items;
    Logger<std::string> logger;
public:
    Inventory() : logger("inventory_log.txt") {}

    size_t size() const { return items.size(); }

    void addItem(std::unique_ptr<Item> item) {
        items.push_back(std::move(item));
        logger.log("Added item: " + items.back()->getName());
    }

    void removeItem(int index) {
        if (index >= 0 && index < items.size()) {
            logger.log("Removed item: " + items[index]->getName());
            items.erase(items.begin() + index);
        }
    }

    void useItem(int index, Character& character) {
        if (index >= 0 && index < items.size()) {
            items[index]->use(character);
            logger.log("Using item: " + items[index]->getName());
            removeItem(index);
        }
    }

    void display() const {
        std::cout << "\nInventory:\n";
        if (items.empty()) {
            std::cout << "Empty\n";
            return;
        }
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << i + 1 << ". " << items[i]->getName() << "\n";
        }
    }
};

class Entity {
protected:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    Entity(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {}

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    void setHealth(int newHealth) { health = newHealth; }
    void setAttack(int newAttack) { attack = newAttack; }

    virtual void attackEnemy(Entity& enemy) = 0;
    virtual void displayInfo() const = 0;
    bool isAlive() const { return health > 0; }
    virtual ~Entity() = default;
};

class Character : public Entity {
private:
    int level;
    int experience;
    Inventory inventory;
    Logger<std::string> logger;

public:
    Character() : Entity("Knight", 100, 15, 5), level(1), experience(0), logger("hero_log.txt") {}

    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    void setLevel(int newLvl) { level = newLvl; }
    void setExperience(int newExp) { experience = newExp; }
    

    void attackEnemy(Entity& enemy) override {
        int damage = attack - enemy.getDefense();
        if (damage > 0) {
            enemy.setHealth(enemy.getHealth() - damage);
            std::cout << name << " attacks " << enemy.getName() << " for " << damage << " damage!" << std::endl;
            logger.log(name + " attacks " + enemy.getName() + " for " + std::to_string(damage) + " damage!");
        }
        else {
            std::cout << name << " attacks " << enemy.getName() << ", but it has no effect!" << std::endl;
            logger.log(name + " attacks " + enemy.getName() + ", but it has no effect!");
        }
    }

    void displayInfo() const override {
        std::cout << "\nName: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense
            << ", Level: " << level << ", Experience: " << experience << std::endl;
    }

    void addToInventory(std::unique_ptr<Item> item) {
        inventory.addItem(std::move(item));
    }

    void useItem(int index) {
        if (index >= 0 && index < inventory.size()) {
            inventory.useItem(index, *this);
        }
    }

    void showInventory() const {
        inventory.display();
    }

    Inventory& getInventory() { return inventory; }

    void saveGame(const std::string& filename) {
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("Unable to save game");
        }

        out << name << "\n" << health << "\n"
            << attack << "\n" << defense << "\n"
            << level << "\n" << experience << "\n";

        logger.log("Game saved for character " + name);
    }

    void loadGame(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("Unable to load game");
        }

        in >> name >> health >> attack >> defense >> level >> experience;

        logger.log("Game loaded for character " + name);
    }

    ~Character() override {}
};

class Grindstone : public Item {
public:
    Grindstone() : Item("Grindstone") {}

    void use(Character& hero) {
        hero.setAttack(hero.getAttack() + 1);
        std::cout << "Use a grindstone!\n" << name << " increase your damage by 1!" << std::endl;
    }

    ~Grindstone() override {}
};

class Potion : public Item {
protected:
    int treatment;
public:
    Potion() : Item("Potion"), treatment(25) {}

    void use(Character& hero) {
        hero.setHealth(hero.getHealth() + treatment);
        if (hero.getHealth() > 100) {
            hero.setHealth(100);
        }
        std::cout << "The potion is drunk!\n" << name << " restored " << treatment << " units of health!\n";
    }

    ~Potion() override {}
};

class Monster : public Entity {
protected:
    int exp;
    Logger<std::string> logger;
public:
    Monster(const std::string& n, int h, int a, int d, int e)
        : Entity(n, h, a, d), exp(e), logger("monster_log.txt") {}


    void displayInfo() const override {
        std::cout << "\nMonster Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    void gainExp(Character& hero) {
        hero.setExperience(hero.getExperience() + exp);
        if (hero.getExperience() >= 100) {
            hero.setLevel(hero.getLevel() + 1);
            hero.setExperience(0);
            hero.setAttack(hero.getAttack() + 1);
            hero.setHealth(100);
            std::cout << hero.getName() << " leveled up to level " << hero.getLevel() << "!" << std::endl;
            logger.log(hero.getName() + " level increased!");

            if (hero.getLevel() % 3 == 0) {
                hero.addToInventory(std::make_unique<Potion>());
                std::cout << "Potion added to inventory.\n";
                logger.log("Potion added to inventory.");
            }
            else if (hero.getLevel() % 2 == 0) {
                hero.addToInventory(std::make_unique<Grindstone>());
                std::cout << "Grindstone added to inventory.\n";
                logger.log("Grindstone added to inventory.");
            }
        }
    }

    ~Monster() override {}
};

class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 30, 8, 3, 25) {}

    void attackEnemy(Entity& hero) override {
        int damage = attack - hero.getDefense();
        if (damage > 0) {
            if (rand() % 100 < 50) {
                hero.setHealth(hero.getHealth() - damage * 2);
                std::cout << "Stab in the back!\n" << name << " attacks " 
                    << hero.getName() << " for " << damage * 2 << " damage!" << std::endl;
                logger.log(name + " attacks " + hero.getName() + " for " + std::to_string(damage * 2) + " damage!");
            }
            else {
                hero.setHealth(hero.getHealth() - damage);
                std::cout << name << " attacks " << hero.getName() << " for " << damage << " damage!" << std::endl;
                logger.log(name + " attacks " + hero.getName() + " for " + std::to_string(damage) + " damage!");
            }

            if (hero.getHealth() <= 0) {
                throw std::runtime_error(hero.getName() + " was killed, the game is over!");
                logger.log("Game over! " + name + " killed the hero!");
            }
        }
        else {
            std::cout << name << " attacks " << hero.getName() << ", but it has no effect!" << std::endl;
            logger.log(name + " attacks " + hero.getName() + ", but it has no effect!");
        }
    }

    ~Goblin() override {}
};

class Sceleton : public Monster {
public:
    Sceleton() : Monster("Sceleton", 20, 7, 2, 15) {}

    void attackEnemy(Entity& hero) override {
        int damage = attack - hero.getDefense();
        if (damage > 0) {
            if (rand() % 100 < 30) {
                hero.setHealth(hero.getHealth() - damage * 3);
                std::cout << "Critical hit!\n" << name << " attacks "
                    << hero.getName() << " for " << damage * 3 << " damage!" << std::endl;
                logger.log(name + " attacks " + hero.getName() + " for " + std::to_string(damage * 3) + " damage!");
            }
            else {
                hero.setHealth(hero.getHealth() - damage);
                std::cout << name << " attacks " << hero.getName() << " for " << damage << " damage!" << std::endl;
                logger.log(name + " attacks " + hero.getName() + " for " + std::to_string(damage) + " damage!");
            }

            if (hero.getHealth() <= 0) {
                std::cout << "Game over!\n" << name << " killed the hero!" << std::endl;
                logger.log("Game over! " + name + " killed the hero!");
            }
        }
        else {
            throw std::runtime_error(hero.getName() + " was killed, the game is over!");
            logger.log(name + " attacks " + hero.getName() + ", but it has no effect!");
        }
    }

    ~Sceleton() override {}
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 60, 20, 8, 50) {}

    void attackEnemy(Entity& hero) override {
        int damage = attack - hero.getDefense();
        if (damage > 0) {
            if (rand() % 100 < 10) {
                hero.setHealth(hero.getHealth() - damage + 10 );
                std::cout << "Fireball!\n" << name << " attacks "
                    << hero.getName() << " for " << damage + 10 << " damage!" << std::endl;
                logger.log(name + " attacks " + hero.getName() + " for " + std::to_string(damage + 10) + " damage!");
            }
            else {
                hero.setHealth(hero.getHealth() - damage);
                std::cout << name << " attacks " << hero.getName() << " for " << damage << " damage!" << std::endl;
                logger.log(name + " attacks " + hero.getName() + " for " + std::to_string(damage) + " damage!");
            }

            if (hero.getHealth() <= 0) {
                std::cout << "Game over!\n" << name << " killed the hero!" << std::endl;
                logger.log("Game over! " + name + " killed the hero!");
            }
        }
        else {
            throw std::runtime_error(hero.getName() + " was killed, the game is over!");
            logger.log(name + " attacks " + hero.getName() + ", but it has no effect!");
        }
    }

    ~Dragon() override {}
};

class Troll : public Monster {
public:
    Troll() : Monster("Troll", 40, 14, 1, 30) {}

    void attackEnemy(Entity& hero) override {
        int damage = attack - hero.getDefense();
        if (damage > 0) {
            if (rand() % 100 < 40) {
                hero.setHealth(hero.getHealth() - damage + 5);
                std::cout << "Heavy blow!\n" << name << " attacks "
                    << hero.getName() << " for " << damage + 5 << " damage!" << std::endl;
                logger.log(name + " attacks " + hero.getName() + " for " + std::to_string(damage + 5) + " damage!");
            }
            else {
                hero.setHealth(hero.getHealth() - damage);
                std::cout << name << " attacks " << hero.getName() << " for " << damage << " damage!" << std::endl;
                logger.log(name + " attacks " + hero.getName() + " for " + std::to_string(damage) + " damage!");
            }

            if (hero.getHealth() <= 0) {
                std::cout << "Game over!\n" << name << " killed the hero!" << std::endl;
                logger.log("Game over! " + name + " killed the hero!");
            }
        }
        else {
            throw std::runtime_error(hero.getName() + " was killed, the game is over!");
            logger.log(name + " attacks " + hero.getName() + ", but it has no effect!");
        }
    }

    ~Troll() override {}
};

class Game {
private:
    std::unique_ptr<Character> player;
    Logger<std::string> logger;
public:
    Game() : logger("game_log.txt") {
        logger.log("Game started");
        player = std::make_unique<Character>();
    }

    void start() {
        player->addToInventory(std::make_unique<Grindstone>());
        player->addToInventory(std::make_unique<Potion>());
        std::cout << "Welcome to the game!";
        menu();
    }

    void menu() {
        while (true) {
            std::cout << "\nMain Menu\n";
            std::cout << "1. Play\n";
            std::cout << "2. Show hero info\n";
            std::cout << "3. Show inventory\n";
            std::cout << "4. Save game\n";
            std::cout << "5. Load game\n";
            std::cout << "6. Exit\n";
            std::cout << "Choose an option: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            try {
                switch (choice) {
                case 1: play(); break;
                case 2: player->displayInfo(); break;
                case 3: player->showInventory(); break;
                case 4:
                    player->saveGame("save.txt");
                    std::cout << "Game saved!\n";
                    break;
                case 5:
                    player->loadGame("save.txt");
                    std::cout << "Game loaded!\n";
                    break;
                case 6: return;
                default: std::cout << "Invalid choice!\n";
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
                if (!player->isAlive()) {
                    std::cout << "Game Over!\n";
                    return;
                }
            }
        }
    }

    void play() {
        std::unique_ptr<Monster> monster;

        int a = rand() % 10;
        if (a < 3) {
            monster = std::make_unique<Sceleton>();
        }
        else if (a < 6) {
            monster = std::make_unique<Goblin>();
        }
        else if (a < 8) {
            monster = std::make_unique<Troll>();
        }
        else{
            monster = std::make_unique<Dragon>();
        }

        std::cout << "\nYou have encountered a " << monster->getName() << ". Attack!\n";
        monster->displayInfo();

        fight(*monster);
    }

    void fight(Monster& monster) {
        logger.log("The beginning of the fight between " + player->getName() + " and " + monster.getName());

        while (player->isAlive() && monster.isAlive()) {
            std::cout << player->getName() << " ===" << " HP: " << player->getHealth() << std::endl;
            std::cout << monster.getName() << " ===" << " HP: " << monster.getHealth() << std::endl;

            std::cout << "\n1. Attack\n";
            std::cout << "2. Use item\n";
            std::cout << "Choose an action: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();
            std::cout << std::endl;

            try {
                switch (choice) {
                case 1:
                    player->attackEnemy(monster);
                    if (monster.isAlive()) {
                        monster.attackEnemy(*player);
                    }
                    else{
                        monster.gainExp(*player);
                    }
                    break;
                case 2:
                    player->showInventory();
                    if (player->getInventory().size() > 0) {
                        std::cout << "Enter item number to use (0 to cancel): ";
                        int itemChoice;
                        std::cin >> itemChoice;
                        std::cin.ignore();
                        if (itemChoice > 0 && itemChoice <= player->getInventory().size()) {
                            player->useItem(itemChoice - 1);
                            monster.attackEnemy(*player);
                        }
                    }
                    else {
                        std::cout << "Inventory is empty!\n";
                    }
                    break;
                default:
                    std::cout << "Invalid choice!\n";
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
                if (!player->isAlive()) {
                    std::cout << "Game Over!\n";
                    return;
                }
            }
        }
        if (player->isAlive()) {
            std::cout << "You defeated the " << monster.getName() << "!\n";
            logger.log(player->getName() + " defeated " + monster.getName());
        }
    }
};

int main() {
    try {
        Game game;
        game.start();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}