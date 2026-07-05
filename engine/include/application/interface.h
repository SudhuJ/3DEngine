#pragma once
#include "context.h"

namespace flow {
    struct appInterface {
        FLOW_INLINE virtual ~appInterface() = default;

        template <typename Layer> FLOW_INLINE Layer* getLayer() {
            FLOW_STATIC_ASSERT(std::is_base_of<appInterface, Layer>::value);
            auto itr = std::find_if(m_Context->Layers.begin(), m_Context->Layers.end(), [this](auto layer) {
                    return layer->m_LayerID == TypeID<Layer>();
            });
            return itr != m_Context->Layers.end() ? static_cast<Layer*>(*itr) : nullptr;
        }

        // attach layer, check if already attached
        template <typename Layer, typename... Args> FLOW_INLINE Layer* attachLayer(Args&&... args) {
            // layer's type compliance check
            FLOW_STATIC_ASSERT(std::is_base_of<appInterface, Layer>::value);
            if (getLayer<Layer>() != nullptr) {
                FLOW_ERROR("Layer already attached.");
                return nullptr;
            }

                // create and attach layer
            auto layer = new Layer(std::forward<Args>(args)...);
            m_Context->Layers.push_back(layer);
            layer->m_LayerID = TypeID<Layer>();
            layer->m_Context = m_Context;
            layer->onStart();
            return layer;
        }

        // detach layer
        template <typename Layer> FLOW_INLINE void detachLayer() {
            // safety check for inheritance from appInterface
            FLOW_STATIC_ASSERT(std::is_base_of<appInterface, Layer>::value);
            m_Context->dispatcher.postTask([this]{
                // removes layer from context and deletes it
                m_Context->Layers.erase(std::remove_if(m_Context->Layers.begin(), m_Context->Layers.end(),
                    [this] (auto layer){
                        if (layer->m_LayerID == TypeID<Layer>()) {
                            m_Context->dispatcher.eraseListener(layer->m_LayerID);
                            FLOW_DELETE(layer);
                            return true;
                        }
                        return false;
                    }
                ), m_Context->Layers.end());
            });
        }

        // attach callback
        template<typename Event, typename Callback>
        FLOW_INLINE void attachCallback(Callback&& callback) {
            m_Context->dispatcher.attachCallback<Event>(std::move(callback), m_LayerID);
        }

        // post event
        template<typename Event, typename... Args>
        FLOW_INLINE void postEvent(Args&&... args) {
            m_Context->dispatcher.postEvent<Event>(std::forward<Args>(args)...);
        }

        // post task event
        template<typename Task>
        FLOW_INLINE void postTask(Task&& task) {
            m_Context->dispatcher.postTask(std::move(task));
        }

        // detach callback
        template<typename Event>
        FLOW_INLINE void detachCallback() {
            m_Context->dispatcher.detachCallback<Event>(m_LayerID);
        }

        // create entity
        template <typename Entt, typename... Args>
        FLOW_INLINE Entt createEntt(Args&&... args) {
            FLOW_STATIC_ASSERT(std::is_base_of<Entity, Entt>::value);
            return std::move(Entt(&m_Context->Scene, std::forward<Args>(args)...));
        }

        template <typename Entt>
        FLOW_INLINE Entt toEntt(entityID entity) {
            FLOW_STATIC_ASSERT(std::is_base_of<Entity, Entt>::value);
            return std::move(Entt(&m_Context->Scene, entity));
        }

        template <typename Entt, typename Comp, typename Task>
        FLOW_INLINE void enttView(Task&& task) {
            FLOW_STATIC_ASSERT(std::is_base_of<Entity, Entt>::value);
            m_Context->Scene.view<Comp>().each([this, &task](auto entity, auto& comp) {
                task(toEntt<Entt>(entity), comp);
            });
        }

        protected:
            FLOW_INLINE virtual void onUpdate() {}
            FLOW_INLINE virtual void onStart() {}

        private:
            friend struct Application;
            appContext* m_Context;
            uint32_t m_LayerID;
    };
}
