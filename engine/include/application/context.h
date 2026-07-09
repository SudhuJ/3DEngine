#pragma once
#include "common/core.h"
#include "common/event.h"

#include "window/window.h"
#include "graphics/renderer.h"
#include "../physics/context.h"

#include "auxillaries/ECS.h"
// #include "interface.h"

namespace flow {
    struct appInterface;

    struct appContext {

        FLOW_INLINE appContext() {
            Window = std::make_unique<appWindow>(&dispatcher, 1280, 720, "Flow Engine");
            Renderer = std::make_unique<graphicsRenderer>(1280, 720);
            Physics = std::make_unique<physicsContext>();
            deltaTime = 0.0;
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
        std::unique_ptr<physicsContext> Physics;
        entityRegistry Scene;
        double deltaTime;

    };
}
