#include <flow.h>
#include "windows/hierarchy.h"
#include "windows/inspector.h"
#include "windows/menubar.h"
#include "windows/resource.h"
#include "windows/viewport.h"

#include "../../engine/include/application/application.h"

using namespace flow;

struct testEvent {
    int32_t data = 0;
};

struct myLayer : public appInterface {

    FLOW_INLINE void onStart() {
        attachCallback<mouseMotionEvent>([this] (auto e){
            FLOW_TRACE("Mouse x: {}, y: {}", e.targetX, e.targetY);
        });
    }

    FLOW_INLINE void onUpdate() {
        FLOW_INFO("onUpdate()");
        if (++m_Counter == 10) {
            postEvent<testEvent>();
        }
    }

    private:
        int32_t m_Counter = 0;
};


struct Editor : GuiContext {
    FLOW_INLINE void onGUIStart() {
        postEvent<SelectEvent>((entityID)4);
        AttachWindow<HierarchyWindow>();
        AttachWindow<InspectorWindow>();
        AttachWindow<ResourceWindow>();
        AttachWindow<MenuBarWindow>();
        AttachWindow<ViewportWindow>();
    }

    FLOW_INLINE void onGUIFrame() {

    }
};

int32_t main(int32_t argc, char* argv[]) {
    FLOW_INFO("Editor Started.");

    auto app = new Application();
    app->attachLayer<Editor>();
    app->runContext();
    // app->~Application();
    FLOW_DELETE(app);
    return 0;
}
