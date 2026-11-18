#include "core/Event.hpp"

#include <algorithm>

namespace ftree::core {

int EventDispatcher::subscribe(EventType type, EventHandler handler) {
    int id = nextHandlerId_++;
    handlers_[type].emplace_back(id, handler);
    return id;
}

void EventDispatcher::unsubscribe(EventType type, int handlerId) {
    auto it = handlers_.find(type);
    if (it != handlers_.end()) {
        auto& list = it->second;
        list.erase(
            std::remove_if(list.begin(), list.end(),
                [handlerId](const std::pair<int, EventHandler>& p) { return p.first == handlerId; }),
            list.end());
    }
}

void EventDispatcher::publish(const Event& event) {
    auto it = handlers_.find(event.getType());
    if (it != handlers_.end()) {
        for (const auto& pair : it->second) {
            pair.second(event);
        }
    }
}

} // namespace ftree::core
