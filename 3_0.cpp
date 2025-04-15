#include <iostream>
#include <string>

class Character {
private:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {}

    // Перегрузка оператора ==
    bool operator==(const Character& other) const {
        return name == other.name && health == other.health;
    }

    // Перегрузка оператора <<
    friend std::ostream& operator<<(std::ostream& os, const Character& character) {
        os << "Character: " << character.name << ", HP: " << character.health
            << ", Attack: " << character.attack << ", Defense: " << character.defense;
        return os;
    }
};

class Weapon {
private:
    std::string name;
    int damage;
public:
    Weapon(const std::string& n, int d) : name(n), damage(d) {}

    Weapon operator+(const Weapon& other) const
    {
        return Weapon(name + other.name, damage + other.damage);
    }

    bool operator>(const Weapon& other) const
    {
        return damage > other.damage;
    }

    friend std::ostream& operator<<(std::ostream& os, const Weapon& weapon)
    {
        os << "Weapon: " << weapon.name << ", Damage: " << weapon.damage;
        return os;
    }

    ~Weapon() {}
};

int main() {
    Character hero1("Hero", 100, 20, 10);
    Character hero2("Hero", 100, 20, 10);
    Character hero3("Warrior", 150, 25, 15);

    if (hero1 == hero2) {
        std::cout << "Hero1 and Hero2 are the same!\n";
    }
    if (!(hero1 == hero3)) {
        std::cout << "Hero1 and Hero3 are different!\n";
    }

    std::cout << hero1 << std::endl; // Вывод информации о персонаже

    Weapon dagger("Dagger", 10);
    Weapon baton("Baton", 5);
    std::cout << dagger << std::endl;
    std::cout << baton << std::endl;

    if (dagger > baton) {
        std::cout << "Dagger damage is more then baton damage!\n";
    }

    Weapon dagbat = dagger + baton;
    std::cout << dagbat << std::endl;
    return 0;
}