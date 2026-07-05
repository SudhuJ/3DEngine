#pragma once
#include "common/core.h"
#include "common/event.h"

#include "window/window.h"
#include "graphics/renderer.h"
#include "auxillaries/ECS.h"


namespace flow {
    struct appInterface;

    struct appContext {

        FLOW_INLINE appContext() {
            Window = std::make_unique<appWindow>(&dispatcher, 1280, 720, "Flow Engine");
            Renderer = std::make_unique<graphicsRenderer>(1280, 720);
        }

        FLOW_INLINE ~appContext() {
            for (auto& layer : Layers) {
                FLOW_DELETE(layer);
            }
            Layers.clear();
        }

        eventDispatcher dispatcher;
        std::vector<appInterface*> Layers;
        std::unique_ptr<appWindow> Window;
        std::unique_ptr<graphicsRenderer> Renderer;
        entityRegistry Scene;
    };
}
