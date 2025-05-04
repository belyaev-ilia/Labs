#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

class Entity
{
protected:
    std::string name;
    int HP;
    int damage;

public:
    Entity(const std::string& n, int h, int d)
        : name(n), HP(h), damage(d) {}

    std::string getName() const { return name; }
    int getHP() const { return HP; }
    int getDamage() const { return damage; }

    void takeDamage(int damage) { HP -= damage; }
    bool isAlive() const { return HP > 0; }

    virtual void attack(Entity& target) = 0;
    virtual void displayInfo() const = 0;
    virtual ~Entity() = default;
};

class Hero : public Entity {
public:
    Hero(const std::string& n, int h, int d)
        : Entity(n, h, d) {}

    void attack(Entity& target) override
    {
        int attackDamage = damage;
        if (rand() % 100 < 30)
        {
            attackDamage *= 2;
            std::cout << "Strong hit! " << name << " attacks " << target.getName() << " for " << damage * 2 << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        target.takeDamage(attackDamage);
    };

    void heal(int amount) {
        if (rand() % 100 < 5)
        {
            HP += amount * 2;
            if (HP > 200){
                HP = 200;
            }
            std::cout << "Big potion! Character healed with " << amount * 2 << " HP\n";
        }
        else {
            HP += amount;
            if (HP > 200) {
                HP = 200;
            }
            std::cout << "Character healed with " << amount << " HP\n";
        }
    }

    void displayInfo() const override
    {
        std::cout << "Hero: " << name << ", HP: " << HP
            << ", Damage: " << damage << std::endl;
    }

    virtual ~Hero() {}
};

class Monster : public Entity
{
public:
    Monster(const std::string& n, int h, int d)
        : Entity(n, h, d) {}

    void attack(Entity& target) override
    {
        int attackDamage = damage;
        if (rand() % 100 < 10)
        {
            attackDamage *= 3;
            std::cout << "Hit in the back! " << name << " attacks " 
                << target.getName() << " for " << damage * 3 << " damage!\n";;
        }
        else {
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        target.takeDamage(attackDamage);
    }

    void displayInfo() const override
    {
        std::cout << "Monster: " << name << ", HP: " << HP
            << ", Attack: " << damage << std::endl;
    }

    virtual ~Monster() {}
};

std::vector<Monster> monsters;
std::mutex monstersMutex;
std::mutex fightMutex;

void generateMonsters() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::lock_guard<std::mutex> lock(monstersMutex);
        monsters.push_back(Monster("Goblin_" + std::to_string(rand() % 100), 50, 25));
        std::cout << "New monster generated!\n";
    }
}
void fight(Hero& hero, Monster& monster) {
    std::cout << "\nBattle of " << hero.getName() << " and " << monster.getName() << "!\n";
    while (hero.isAlive() && monster.isAlive()) {
        std::lock_guard<std::mutex> lock(fightMutex);

        hero.attack(monster);
        if (!monster.isAlive()) {
            std::cout << monster.getName() << " defeated!\n";
            break;
        }

        monster.attack(hero);
        if (!hero.isAlive()) {
            std::cout << hero.getName() << " defeated!\n";
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if (hero.isAlive()) {
        hero.heal(15);
    }
}

int main() {
    Hero hero("Knight", 80, 25);

    std::thread monsterGenerator(generateMonsters);
    monsterGenerator.detach();

    while (hero.isAlive()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::lock_guard<std::mutex> lock(monstersMutex);
        if (!monsters.empty()) {
            Monster currentMonster = monsters[0];
            monsters.erase(monsters.begin());
            std::thread fightThread(fight, std::ref(hero), std::ref(currentMonster));
            fightThread.join();
            std::cout << "\nHero indicators:\n";
            hero.displayInfo();
        }
    }
    std::cout << "Game Over!\n";
    return 0;
}