#ifndef EVENT_DISPATCHER_HPP
#define EVENT_DISPATCHER_HPP

#include <functional>
#include <vector>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

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

class Event {
public:
    Event(EventType type) : type(type) {}
    
    void setData(const std::string& key, const std::string& value);
    
    std::optional<std::string> getData(const std::string& key) const;
    
    std::string getDataOrDefault(const std::string& key, const std::string& defaultValue = "") const;
    
    EventType getType() const;
    
private:
    EventType type;
    std::unordered_map<std::string, std::string> data;
};

class EventDispatcher {
public:
    using EventHandler = std::function<void(const Event&)>;
    using HandlerId = int;
    
    HandlerId subscribe(EventType type, EventHandler handler);
    void unsubscribe(EventType type, HandlerId handlerId);
    void publish(const Event& event);
    
private:
    std::unordered_map<EventType, std::vector<std::pair<HandlerId, EventHandler>>> handlers;
    std::mutex mutex;
    HandlerId nextHandlerId = 1;
};

#endif