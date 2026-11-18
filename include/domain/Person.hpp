#pragma once

#include <string>
#include <cstdint>

namespace ftree::domain {

using PersonId = uint32_t;
using FamilyId = uint32_t;

enum class Gender : uint8_t { Male, Female };

// Data structure for person (used in World)
struct PersonData {
    PersonId id = 0;
    std::string name;
    Gender gender = Gender::Male;
    int age = 0;
    bool alive = true;
    bool pregnant = false;
    PersonId motherId = 0;
    PersonId fatherId = 0;
    FamilyId familyId = 0;
};

class Person {
public:
    // Default constructor
    Person() : id_(0), name_(""), gender_(Gender::Male), age_(0), alive_(true), pregnant_(false),
               motherId_(0), fatherId_(0), familyId_(0) {}

    // Constructor with old interface: Person(int uuid, Gender gender, int age)
    Person(int uuid, Gender gender, int age)
        : id_(uuid), name_(""), gender_(gender), age_(age), alive_(true), pregnant_(false),
        motherId_(0), fatherId_(0), familyId_(0) {}

    // Constructor with new interface: Person(PersonId id, const std::string& name, Gender gender, int age = 0)
    Person(PersonId id, const std::string& name, Gender gender, int age = 0)
        : id_(id), name_(name), gender_(gender), age_(age), alive_(true), pregnant_(false),
        motherId_(0), fatherId_(0), familyId_(0) {}

    // Full constructor
    Person(PersonId id, const std::string& name, Gender gender, int age,
           bool alive, bool pregnant,
           PersonId motherId = 0, PersonId fatherId = 0,
           FamilyId familyId = 0)
        : id_(id), name_(name), gender_(gender), age_(age),
          alive_(alive),
          pregnant_(alive && gender == Gender::Female && pregnant),
          motherId_(motherId), fatherId_(fatherId), familyId_(familyId) {}

    // State transitions. Random decisions belong to simulation systems.
    void incrementAge() {
        if (alive_) {
            ++age_;
        }
    }

    void setPregnant(bool isPregnant) {
        pregnant_ = isPregnant && alive_ && gender_ == Gender::Female;
    }

    void setAlive(bool isAlive) {
        alive_ = isAlive;
        if (!alive_) {
            pregnant_ = false;
        }
    }

    // Eligibility only; probability and random sampling belong to PregnancySystem.
    bool canConceive() const {
        return alive_ && gender_ == Gender::Female && !pregnant_
            && age_ >= 18 && age_ <= 45;
    }

    // New features
    PersonId getId() const { return id_; }
    const std::string& getName() const { return name_; }
    Gender getGender() const { return gender_; }
    int getAge() const { return age_; }
    bool isAlive() const { return alive_; }
    bool isPregnant() const { return pregnant_; }
    PersonId getMotherId() const { return motherId_; }
    PersonId getFatherId() const { return fatherId_; }
    FamilyId getFamilyId() const { return familyId_; }

    void setName(const std::string& name) { name_ = name; }
    void setAge(int age) { age_ = age; }

private:
    PersonId id_ = 0;
    std::string name_;
    Gender gender_ = Gender::Male;
    int age_ = 0;
    bool alive_ = true;
    bool pregnant_ = false;
    PersonId motherId_ = 0;
    PersonId fatherId_ = 0;
    FamilyId familyId_ = 0;
};

} // namespace ftree::domain
