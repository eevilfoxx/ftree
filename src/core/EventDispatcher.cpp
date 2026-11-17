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
    // Copy handlers while holding the mutex, then call them outside the lock.
    // This makes publish() reentrant-safe: handlers can safely call
    // subscribe/unsubscribe/publish without deadlocking on a non-recursive mutex.
    std::vector<EventHandler> handlersToCall;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = handlers.find(event.getType());
        if (it != handlers.end()) {
            handlersToCall.reserve(it->second.size());
            for (const auto& pair : it->second) {
                handlersToCall.push_back(pair.second);
            }
        }
    }

    for (const auto& handler : handlersToCall) {
        handler(event);
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