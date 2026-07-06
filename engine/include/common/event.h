#pragma once
#include "core.h"

namespace flow{
    template <typename Event>
    struct eventListener {
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

    struct eventRegistryBase {
        virtual ~eventRegistryBase() = default;
        virtual void poll() = 0;
        virtual void erase(uint32_t listenerID) = 0;
    };

    template <typename Event>
    struct eventRegistry : eventRegistryBase {
        using listener = std::unique_ptr<eventListener<Event>>;
        std::queue<std::unique_ptr<Event>> Queue;
        std::vector<listener> Listeners;

        FLOW_INLINE void poll() override {
            while (!Queue.empty()) {
                for (auto& l : Listeners)
                    l->callback(*Queue.front());
                Queue.pop();
            }
        }

        FLOW_INLINE void erase(uint32_t listenerID) override {
            Listeners.erase(std::remove_if(Listeners.begin(), Listeners.end(), [&](auto& l) {
                return l->ID == listenerID;
            }), Listeners.end());
        }
    };

    struct eventDispatcher {
        FLOW_INLINE ~eventDispatcher() = default;

        template <typename Event, typename Callback>
        FLOW_INLINE void attachCallback(Callback&& callback, uint32_t listenerID) {
            auto listener = std::make_unique<eventListener<Event>>(std::move(callback), listenerID);
            getRegistry<Event>()->Listeners.push_back(std::move(listener));
        }

        template <typename Event>
        FLOW_INLINE void detachCallback(uint32_t listenerID) {
            auto& listeners = getRegistry<Event>()->Listeners;
            listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&](auto& listener){
                return listener->ID == listenerID;
            }),
            listeners.end());
        }

        FLOW_INLINE void eraseListener(uint32_t listenerID) {
            for (auto& [_, reg] : m_Registry)
                reg->erase(listenerID);
        }

        template <typename Event, typename... Args>
        FLOW_INLINE void postEvent(Args&&... args) {
            auto registry = getRegistry<Event>();
            if (registry->Listeners.empty())
                return;
            registry->Queue.push(std::make_unique<Event>(std::forward<Args>(args)...));
        }

        template <typename Task>
        FLOW_INLINE void postTask(Task&& task) {
            m_Tasks.push(std::move(task));
        }

        FLOW_INLINE void pollEvents() {
            for (auto& [_, reg] : m_Registry)
                reg->poll();
            while (!m_Tasks.empty()) {
                m_Tasks.front()();
                m_Tasks.pop();
            }
        }

        private:
            template<typename Event>
            FLOW_INLINE eventRegistry<Event>* getRegistry() {
                auto it = m_Registry.find(TypeID<Event>());
                if (it != m_Registry.end()) {
                    return static_cast<eventRegistry<Event>*>(it->second.get());
                }
                auto registry = std::make_unique<eventRegistry<Event>>();
                auto ptr = registry.get();
                m_Registry[TypeID<Event>()] = std::move(registry);
                return ptr;
            }

        private:
            std::unordered_map<uint32_t, std::unique_ptr<eventRegistryBase>> m_Registry;
            std::queue<std::function<void()>> m_Tasks;
    };
}
