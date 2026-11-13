// Person.hpp
#ifndef PERSON_HPP
#define PERSON_HPP

#include <memory>
#include <string>
#include <functional>
#include "Gender.hpp"

class Family;

class Person : public std::enable_shared_from_this<Person> {
public:
    Person(int uuid, Gender gender, int age = 0);
    Person(int uuid, Gender gender, std::shared_ptr<Person> mother, 
           std::shared_ptr<Person> father, int age = 0);
    
    // Геттеры
    int getUuid() const { return uuid; }
    Gender getGender() const { return gender; }
    int getAge() const { return age; }
    bool isPregnant() const { return pregnant; }
    bool isAlive() const { return alive; }
    std::string getName() const { return name; }
    bool isMarried() const;
    
    std::shared_ptr<Person> getMother() const { return mother.lock(); }
    std::shared_ptr<Person> getFather() const { return father.lock(); }
    std::shared_ptr<Family> getFamily() const { return family.lock(); }
    
    // Сеттеры
    void setFamily(std::shared_ptr<Family> family);
    // void setName(const std::string& name) { this->name = name; }
    
    // Действия
    void incrementAge();
    void setPregnant(bool isPregnant);
    void setAlive(bool isAlive);
    
    // Вероятностные события
    bool tryConceive(int partnerAge);
    bool tryDie();
    
private:
    const int uuid;
    const Gender gender;
    const std::weak_ptr<Person> mother;
    const std::weak_ptr<Person> father;
    std::weak_ptr<Family> family;
    
    int age;
    bool pregnant;
    bool alive;
    std::string name;
};

#endif