#pragma once
#include "common/core.h"
#include "common/event.h"

#include "window/window.h"
#include "graphics/renderer.h"
#include "physics/context.h"
#include "scripts/context.h"

#include "auxillaries/ECS.h"
#include "auxillaries/assets.h"
#include "auxillaries/serializer.h"
// #include "interface.h"

namespace flow {
    struct appInterface;

    struct appContext {

        FLOW_INLINE appContext() {
            Window = std::make_unique<appWindow>(&dispatcher, 1280, 720, "Flow Engine");
            Scripts = std::make_unique<ScriptContext>(&Scene, Window.get());
            Renderer = std::make_unique<graphicsRenderer>(1280, 720);
            Physics = std::make_unique<physicsContext>();
            Assets = std::make_unique<AssetRegistry>();
            Serializer = std::make_unique<DataSerializer>();

            deltaTime = 0.0;
        }

        FLOW_INLINE ~appContext() {
            // for (auto& layer : Layers) {
            //     FLOW_DELETE(layer);
            // }
            Layers.clear();
        }

        eventDispatcher dispatcher;
        std::vector<appInterface*> Layers;
        std::unique_ptr<appWindow> Window;
        std::unique_ptr<graphicsRenderer> Renderer;
        std::unique_ptr<physicsContext> Physics;
        std::unique_ptr<ScriptContext> Scripts;
        std::unique_ptr<AssetRegistry> Assets;
        std::unique_ptr<DataSerializer> Serializer;
        entityRegistry Scene;
        double deltaTime;

    };
}
