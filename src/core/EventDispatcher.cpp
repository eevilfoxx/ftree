#include "core/EventDispatcher.hpp"
#include <algorithm>

EventDispatcher::HandlerId EventDispatcher::subscribe(EventType type, EventHandler handler) {
    std::lock_guard<std::mutex> lock(mutex);
    HandlerId id = nextHandlerId++;
    handlers[type].emplace_back(id, std::move(handler));
    return id;
}

void EventDispatcher::unsubscribe(EventType type, HandlerId handlerId) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = handlers.find(type);
    if (it != handlers.end()) {
        auto& handlersForType = it->second;
        handlersForType.erase(
            std::remove_if(handlersForType.begin(), handlersForType.end(),
                [handlerId](const std::pair<HandlerId, EventHandler>& pair) {
                    return pair.first == handlerId;
                }),
            handlersForType.end());
    }
}

void EventDispatcher::publish(const Event& event) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = handlers.find(event.getType());
    if (it != handlers.end()) {
        for (const auto& pair : it->second) {
            pair.second(event);
        }
    }
}

void Event::setData(const std::string& key, const std::string& value) {
        data[key] = value;
    }
    
    std::optional<std::string> Event::getData(const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    std::string Event::getDataOrDefault(const std::string& key, const std::string& defaultValue) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return defaultValue;
    }

    EventType Event::getType() const { return type; }