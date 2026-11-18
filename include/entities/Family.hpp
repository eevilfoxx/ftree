// Family.hpp
#ifndef FAMILY_HPP
#define FAMILY_HPP

#include <memory>
#include <vector>
#include <string>
#include <entities/Person.hpp>

class Family : public std::enable_shared_from_this<Family> {
public:
    Family(int id, std::shared_ptr<Person> mother, std::shared_ptr<Person> father);
    
    // Геттеры
    int getId() const { return id; }
    std::shared_ptr<Person> getMother() const { return mother.lock(); }
    std::shared_ptr<Person> getFather() const { return father.lock(); }
    std::vector<std::shared_ptr<Person>> getChildren() const;
    const std::string& getSurname() const { return surname; }
    
    // Сеттеры
    void setSurname(const std::string& surname) { this->surname = surname; }
    
    // Управление детьми
    void addChild(std::shared_ptr<Person> child);
    void removeChild(std::shared_ptr<Person> child);
    bool hasChild(std::shared_ptr<Person> child) const;
    
private:
    const int id;
    const std::weak_ptr<Person> mother;
    const std::weak_ptr<Person> father;
    std::vector<std::weak_ptr<Person>> children;
    std::string surname;
};

#endif