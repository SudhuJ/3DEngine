#pragma once
#include "core.h"

namespace flow{
    template <typename Event>
    struct eventListener {
        // type alias using callbackFn; std::function<void (const Event&)>
        // allows to store a function pointer or lambda or class member as a callback
        using callbackFn = std::function<void (const Event&)>;
        FLOW_INLINE eventListener(callbackFn&& callback, uint32_t listenerID):
            callback(std::move(callback)), ID(listenerID) {}


        callbackFn callback;
        uint32_t ID;

        template <typename... Args>
        FLOW_INLINE void invokeCallback(const Event& event) const {
            callback(event);
        }
    };

    // event registry
    template <typename Event> struct eventRegistry {
        using listener = std::unique_ptr<eventListener<Event>>;
        std::queue<std::unique_ptr<Event>> Queue;
        std::vector<listener> Listeners;
    };

    // event dispatcher
    struct eventDispatcher {
        FLOW_INLINE ~eventDispatcher() {
            for (auto& [_, ptr] : m_Registry) {
                auto registry = castRegistry<char>(ptr);
                FLOW_DELETE(registry);
            }
        }

        template <typename Event, typename Callback>
        FLOW_INLINE void attachCallback(Callback&& callback, uint32_t listenerID) {
            auto listener = std::make_unique<eventListener<Event>>(std::move(callback), listenerID);
            getRegistry<Event>()->Listeners.push_back(std::move(listener));
        }

        template <typename Event>
        FLOW_INLINE void detachCallback(uint32_t listenerID){
            auto& listeners = getRegistry<Event>()->Listeners;
            listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&](auto& listener){
                return listener->ID == listenerID;
            }),
            listeners.end());
        }

        FLOW_INLINE void eraseListener(uint32_t listenerID) {
            for (auto& [_, registry] : m_Registry) {
                auto& listeners = castRegistry<int8_t>(registry)->Listeners;
                listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&](auto& listener){
                    return listener->ID == listenerID;
                }),
                listeners.end());
            }
        }

        template <typename Event, typename... Args>
        FLOW_INLINE void postEvent(Args&&... args) {
            auto registry = getRegistry<Event>();
            if (registry->Listeners.empty()){
            return;
            }
            registry->Queue.push(std::make_unique<Event>(std::forward<Args>(args)...));
        }

        template <typename Task>
        FLOW_INLINE void postTask(Task&& task) {
            m_Tasks.push(std::move(task));
        }

        FLOW_INLINE void pollEvents() {
            // persistent callbacks
            for (auto& [_, pointer] : m_Registry) {
                auto registry = castRegistry<char>(pointer);
                while (!registry->Queue.empty()) {
                    for (auto& listener : registry->Listeners){
                        listener->callback(*registry->Queue.front());
                    }
                    registry->Queue.pop();
                }
            }
            // frame callbacks
            while (!m_Tasks.empty()) {
                m_Tasks.front()();
                m_Tasks.pop();
            }
        }

        private:
            template<typename Event>
            FLOW_INLINE eventRegistry<Event>* castRegistry(void* ptr) {
                return static_cast<eventRegistry<Event>*>(ptr);
            }

            template<typename Event>
            FLOW_INLINE eventRegistry<Event>* getRegistry() {
                auto it = m_Registry.find(TypeID<Event>());
                if (it != m_Registry.end()) {
                    return castRegistry<Event>(it->second);
                }
                auto registry = new eventRegistry<Event>();
                m_Registry[TypeID<Event>()] = registry;
                return registry;
            }

        private:
            std::unordered_map<uint32_t, void*> m_Registry;
            std::queue<std::function<void()>> m_Tasks;
    };
}
