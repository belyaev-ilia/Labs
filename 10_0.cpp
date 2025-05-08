#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <stdexcept>

class User;
class Resource;
template <typename T, typename U> class AccessControlSystem;

class User {
private:
    std::string name;
    int id;
    int accessLevel;

public:
    User(std::string name, int id, int accessLevel) : name(name), id(id), accessLevel(accessLevel) {
        if (name.empty()) {
            throw std::invalid_argument("Username cannot be empty.");
        }
        if (id < 0) {
            throw std::invalid_argument("User ID cannot be negative.");
        }
        if (accessLevel < 0) {
            throw std::invalid_argument("Access level cannot be negative.");
        }
    }

    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    void setName(std::string newName) {
        if (newName.empty()) {
            throw std::invalid_argument("Username cannot be empty.");
        }
        name = newName;
    }

    void setId(int newId) {
        if (newId < 0) {
            throw std::invalid_argument("User ID cannot be negative.");
        }
        id = newId;
    }

    void setAccessLevel(int newAccessLevel) {
        if (newAccessLevel < 0) {
            throw std::invalid_argument("Access level cannot be negative.");
        }
        accessLevel = newAccessLevel;
    }

    virtual void displayInfo() const {
        std::cout << "Name: " << name << std::endl;
        std::cout << "ID: " << id << std::endl;
        std::cout << "Access level: " << accessLevel << std::endl;
    }
    virtual ~User() {}
};

class Student : public User {
private:
    std::string group;

public:
    Student(std::string name, int id, int accessLevel, std::string group) : User(name, id, accessLevel), group(group) {}

    std::string getGroup() const { return group; }
    void setGroup(std::string newGroup) { group = newGroup; }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << "Group: " << group << std::endl;
    }
};

class Teacher : public User {
private:
    std::string department;

public:
    Teacher(std::string name, int id, int accessLevel, std::string department) : User(name, id, accessLevel), department(department) {}

    std::string getDepartment() const { return department; }
    void setDepartment(std::string newDepartment) { department = newDepartment; }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << "Department: " << department << std::endl;
    }
};

class Administrator : public User {
private:
    std::string position;

public:
    Administrator(std::string name, int id, int accessLevel, std::string position) : User(name, id, accessLevel), position(position) {}

    std::string getPosition() const { return position; }
    void setPosition(std::string newPosition) { position = newPosition; }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << "Job title: " << position << std::endl;
    }
};

class Resource {
private:
    std::string name;
    int requiredAccessLevel;

public:
    Resource(std::string name, int requiredAccessLevel) : name(name), requiredAccessLevel(requiredAccessLevel) {
        if (requiredAccessLevel < 0) {
            throw std::invalid_argument("The access level to a resource cannot be negative..");
        }
    }

    std::string getName() const { return name; }
    int getRequiredAccessLevel() const { return requiredAccessLevel; }

    void setName(std::string newName) { name = newName; }
    void setRequiredAccessLevel(int newRequiredAccessLevel) {
        if (newRequiredAccessLevel < 0) {
            throw std::invalid_argument("The access level to a resource cannot be negative..");
        }
        requiredAccessLevel = newRequiredAccessLevel;
    }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void displayInfo() const {
        std::cout << "Resource: " << name << std::endl;
        std::cout << "Required access level: " << requiredAccessLevel << std::endl;
    }
};

template <typename T, typename U>
class AccessControlSystem {
private:
    std::vector<std::shared_ptr<T>> users;
    std::vector<std::shared_ptr<U>> resources;

public:
    void addUser(std::shared_ptr<T> user) {
        users.push_back(user);
    }

    void addResource(std::shared_ptr<U> resource) {
        resources.push_back(resource);
    }

    bool checkAccess(const T& user, const U& resource) const {
        return resource.checkAccess(user);
    }

    std::shared_ptr<T> findUserByName(const std::string& name) const {
        for (const auto& user : users) {
            if (user->getName() == name) {
                return user;
            }
        }
        return nullptr;
    }

    std::shared_ptr<T> findUserById(int id) const {
        for (const auto& user : users) {
            if (user->getId() == id) {
                return user;
            }
        }
        return nullptr;
    }

    void sortUsersByAccessLevel() {
        std::sort(users.begin(), users.end(), [](const std::shared_ptr<T>& a, const std::shared_ptr<T>& b) {
            return a->getAccessLevel() < b->getAccessLevel();
            });
    }

    const std::vector<std::shared_ptr<T>>& getUsers() const {
        return users;
    }

    const std::vector<std::shared_ptr<U>>& getResources() const {
        return resources;
    }

    void clearUsers() {
        users.clear();
    }

    void clearResources() {
        resources.clear();
    }

    void displayAllUsers() const {
        for (const auto& user : users) {
            user->displayInfo();
            std::cout << "---------------------" << std::endl;
        }
    }

    void displayAllResources() const {
        for (const auto& resource : resources) {
            resource->displayInfo();
            std::cout << "---------------------" << std::endl;
        }
    }
};

template <typename T>
void saveUsersToFile(const std::string& filename, const AccessControlSystem<T, Resource>& system) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for saving.");
    }

    for (const auto& user : system.getUsers()) {
        if (auto student = std::dynamic_pointer_cast<Student>(user)) {
            file << "Student," << student->getName() << "," << student->getId() 
                << "," << student->getAccessLevel() << "," << student->getGroup() << std::endl;
        }
        else if (auto teacher = std::dynamic_pointer_cast<Teacher>(user)) {
            file << "Teacher," << teacher->getName() << "," << teacher->getId() 
                << "," << teacher->getAccessLevel() << "," << teacher->getDepartment() << std::endl;
        }
        else if (auto administrator = std::dynamic_pointer_cast<Administrator>(user)) {
            file << "Administrator," << administrator->getName() << "," << administrator->getId() 
                << "," << administrator->getAccessLevel() << "," << administrator->getPosition() << std::endl;
        }
        else {
            std::cerr << "Unknown user type. Failed to save." << std::endl;
        }
    }

    file.close();
}

template <typename T>
void loadUsersFromFile(const std::string& filename, AccessControlSystem<T, Resource>& system) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for download.");
    }

    system.clearUsers();

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        std::getline(ss, type, ',');

        std::string name;
        std::getline(ss, name, ',');

        int id;
        std::string idStr;
        std::getline(ss, idStr, ',');
        try {
            id = std::stoi(idStr);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Error reading user ID: " << e.what() << std::endl;
            continue;
        }

        int accessLevel;
        std::string accessLevelStr;
        std::getline(ss, accessLevelStr, ',');
        try {
            accessLevel = std::stoi(accessLevelStr);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Error reading user access level: " << e.what() << std::endl;
            continue;
        }

        if (type == "Student") {
            std::string group;
            std::getline(ss, group, ',');
            system.addUser(std::make_shared<Student>(name, id, accessLevel, group));
        }
        else if (type == "Teacher") {
            std::string department;
            std::getline(ss, department, ',');
            system.addUser(std::make_shared<Teacher>(name, id, accessLevel, department));
        }
        else if (type == "Administrator") {
            std::string position;
            std::getline(ss, position, ',');
            system.addUser(std::make_shared<Administrator>(name, id, accessLevel, position));
        }
        else {
            std::cerr << "Unknown user type in file: " << type << std::endl;
        }
    }

    file.close();
}

void saveResourcesToFile(const std::string& filename, const AccessControlSystem<User, Resource>& system) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file to save resources.");
    }

    for (const auto& resource : system.getResources()) {
        file << resource->getName() << "," << resource->getRequiredAccessLevel() << std::endl;
    }

    file.close();
}

void loadResourcesFromFile(const std::string& filename, AccessControlSystem<User, Resource>& system) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file to load resources.");
    }

    system.clearResources();

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name;
        std::getline(ss, name, ',');

        int requiredAccessLevel;
        std::string accessLevelStr;
        std::getline(ss, accessLevelStr, ',');
        try {
            requiredAccessLevel = std::stoi(accessLevelStr);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Error reading resource access level: " << e.what() << std::endl;
            continue;
        }

        system.addResource(std::make_shared<Resource>(name, requiredAccessLevel));
    }

    file.close();
}


int main() {
    AccessControlSystem<User, Resource> system;

    try {
        std::shared_ptr<Student> student1 = std::make_shared<Student>("Ivan", 123, 1, "Group 1");
        std::shared_ptr<Teacher> teacher1 = std::make_shared<Teacher>("Nikolay", 456, 5, "Computer Science");
        std::shared_ptr<Administrator> admin1 = std::make_shared<Administrator>("Besarion", 789, 10, "Rector");

        system.addUser(student1);
        system.addUser(teacher1);
        system.addUser(admin1);

        std::shared_ptr<Resource> lab1 = std::make_shared<Resource>("Lab 101", 3);
        std::shared_ptr<Resource> library = std::make_shared<Resource>("Library", 1);
        std::shared_ptr<Resource> rectorOffice = std::make_shared<Resource>("Rector's Office", 8);

        system.addResource(lab1);
        system.addResource(library);
        system.addResource(rectorOffice);

        std::cout << "User information:" << std::endl;
        system.displayAllUsers();

        std::cout << "Resource Information:" << std::endl;
        system.displayAllResources();

        std::cout << "\nAccess check:" << std::endl;
        if (system.checkAccess(*student1, *lab1)) {
            std::cout << student1->getName() << " has access to " << lab1->getName() << std::endl;
        }
        else {
            std::cout << student1->getName() << " does not have access to " << lab1->getName() << std::endl;
        }

        if (system.checkAccess(*teacher1, *lab1)) {
            std::cout << teacher1->getName() << " has access to " << lab1->getName() << std::endl;
        }
        else {
            std::cout << teacher1->getName() << " does not have access to " << lab1->getName() << std::endl;
        }

        if (system.checkAccess(*admin1, *rectorOffice)) {
            std::cout << admin1->getName() << " has access to " << rectorOffice->getName() << std::endl;
        }
        else {
            std::cout << admin1->getName() << " does not have access to " << rectorOffice->getName() << std::endl;
        }

        std::cout << "\nSaving data to files..." << std::endl;
        saveUsersToFile("users.txt", system);
        saveResourcesToFile("resources.txt", system);

        std::cout << "\nLoading data from files..." << std::endl;
        AccessControlSystem<User, Resource> newSystem;
        loadUsersFromFile("users.txt", newSystem);
        loadResourcesFromFile("resources.txt", newSystem);

        std::cout << "\nUser information after loading:" << std::endl;
        newSystem.displayAllUsers();

        std::cout << "Information about resources after loading:" << std::endl;
        newSystem.displayAllResources();

        std::cout << "\nSearch user by name 'Ivan':" << std::endl;
        std::shared_ptr<User> foundUser = newSystem.findUserByName("Ivan");
        if (foundUser) {
            foundUser->displayInfo();
        }
        else {
            std::cout << "User not found." << std::endl;
        }

        std::cout << "\nSort users by access level:" << std::endl;
        newSystem.sortUsersByAccessLevel();
        newSystem.displayAllUsers();

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
