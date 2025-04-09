#include <iostream>
#include <string>

class Entity
{
protected:
    std::string name;
    int health;
    int attack;
    int defense;
public:
    Entity(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {}

    // Виртуальный метод для атаки
    virtual void attackEnemy(Entity& target)
    {
        int damage = attack - target.defense;
        if (damage > 0)
        {
            target.health -= damage;
            std::cout << name << " attacks " << target.name << " for " << damage << " damage!\n";
        }
        else
        {
            std::cout << name << " attacks " << target.name << ", but it has no effect!\n";
        }
    }
    //Восстановение здоровья
    virtual void heal(int amount) {
        health += amount;
    }

    //Геттер для защиты
    int getDefence() const { 
        return defense; 
    }

    //Геттер для имени
    std::string getName() const { 
        return name; 
    }
    
    //Получение урона
    void takeDamage(int damage) { 
        health -= damage; 
    }

    // Виртуальный метод для вывода информации
    virtual void displayInfo() const
    {
        std::cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    // Виртуальный деструктор
    virtual ~Entity() {}
};
class Character : public Entity
{
public:
    Character(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {}

    // Переопределение метода attack
    void attackEnemy(Entity& target) override
    {
        int damage = attack - target.getDefence();
        if (damage > 0)
        {
            // Шанс на критический удар (20%)
            if (rand() % 100 < 20)
            {
                damage *= 2;
                std::cout << "Critical hit! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        else
        {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    //Переопределение метода heal
    void heal(int amount) {
        if (health < 100) {
            health += amount;
            if (health > 100) {
                health = 100;
            }
        }
    }

    // Переопределение метода displayInfo
    void displayInfo() const override
    {
        std::cout << "Character: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};

class Monster : public Entity
{
public:
    Monster(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {}

    // Переопределение метода attack
    void attackEnemy(Entity& target) override
    {
        int damage = attack - target.getDefence();
        if (damage > 0)
        {
            // Шанс на ядовитую атаку (30%)
            if (rand() % 100 < 30)
            {
                damage += 5; // Дополнительный урон от яда
                std::cout << "Poisonous attack! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        else
        {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    // Переопределение метода displayInfo
    void displayInfo() const override
    {
        std::cout << "Monster: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};
class Boss : public Monster
{
private:
    std::string specialAbility;
    int specialAbility_damage;

public:
    Boss(const std::string& n, int h, int a, int d, const std::string& sa, int sa_d)
        : Monster(n, h, a, d), specialAbility(sa), specialAbility_damage(sa_d) {}
    void attackEnemy(Entity& target)
    {
        int damage = attack + specialAbility_damage;
        target.takeDamage(damage);
        std::cout << "Boss used Special Ability! It takes " << damage << " damage" << std::endl;
    }

    // Переопределение метода displayInfo
    void displayInfo() const override
    {
        std::cout << "Boss: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Special ability: " << specialAbility << ", Defense: " << defense << std::endl;
    }
};

int main() {
    srand(static_cast<unsigned>(time(0))); // Инициализация генератора случайных чисел

    // Создание объектов
    Character hero("Hero", 100, 20, 10);
    Monster goblin("Goblin", 50, 15, 5);
    Monster dragon("Dragon", 150, 25, 20);
    Boss boss("Knight", 400, 20, 50, "Powerful blow", 40);

    // Массив указателей на базовый класс
    Entity* entities[] = { &hero, &goblin, &dragon, &boss };

    // Полиморфное поведение
    for (auto& entity : entities) {
        entity->displayInfo(); // Вывод информации о сущности
    }

    // Бой между персонажем и монстрами
    hero.attackEnemy(goblin);
    goblin.attackEnemy(hero);
    dragon.attackEnemy(hero);
    hero.displayInfo();
    hero.heal(30);
    hero.displayInfo();
    boss.attackEnemy(hero);
    hero.displayInfo();

    return 0;
}