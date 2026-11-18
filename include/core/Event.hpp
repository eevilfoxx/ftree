#pragma once

#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <optional>
#include <memory>

#include "domain/Person.hpp"

namespace ftree::core {

using domain::PersonId;
using domain::FamilyId;

// Type-safe event data structures
struct PersonBornData {
    PersonId personId;
    std::string personName;
    PersonId parentId;  // Optional
    std::string familyName;
};

struct PersonDiedData {
    PersonId personId;
    std::string personName;
    int personAge;
    std::string deathCause;
};

struct PersonMovedData {
    FamilyId familyId;
    PersonId childId;
};

struct MarriageData {
    PersonId maleId;
    PersonId femaleId;
    std::string maleName;
    std::string femaleName;
    // Marriage date could be added later
};

struct PregnancyData {
    PersonId motherId;
    std::string motherName;
    int motherAge;
    PersonId fatherId;
    std::string fatherName;
    int fatherAge;
};

struct ChildbirthData {
    PersonId childId;
    PersonId motherId;
    PersonId fatherId;
    std::string childName;
    std::string familyName;
};

struct FamilyCreatedData {
    FamilyId familyId;
    PersonId motherId;
    std::string motherName;
    PersonId fatherId;
    std::string fatherName;
    std::string familySurname;
};

struct FamilyDissolvedData {
    FamilyId familyId;
    PersonId motherId;
    std::string motherName;
    PersonId fatherId;
    std::string fatherName;
};

// Event type enum
enum class EventType {
    PersonBorn,
    PersonDied,
    PersonMoved,
    Marriage,
    Pregnancy,
    Childbirth,
    FamilyCreated,
    FamilyDissolved
};

// Event class with typed data
class Event {
public:
    Event(EventType type) : type_(type) {}
    
    EventType getType() const { return type_; }
    
    // Get data by type-specific struct
    // In a full implementation would use variant or overloaded getters
    
private:
    EventType type_;
};

// Event handler type
using EventHandler = std::function<void(const Event&)>;

class EventDispatcher {
public:
    // Subscribe handler for event type
    // Returns id for unsubscription
    int subscribe(EventType type, EventHandler handler);
    
    // Unsubscribe handler
    void unsubscribe(EventType type, int handlerId);
    
    // Publish event
    void publish(const Event& event);
    
private:
    // Map from event type to list of (handlerId, handler)
    using HandlerList = std::vector<std::pair<int, EventHandler>>;
    std::unordered_map<EventType, HandlerList> handlers_;
    int nextHandlerId_ = 1;
};

} // namespace ftree::core
