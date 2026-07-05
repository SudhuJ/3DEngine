#include <flow.h>

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

int32_t main(int32_t argc, char* argv[]) {
    FLOW_INFO("Editor Started.");

    auto app = new Application();
    app->attachCallback<keyPressEvent>([](auto e) {
        FLOW_INFO("Key pressed: {}", e.key);
    });
    app->attachLayer<myLayer>();
    app->runContext();
    return 0;
}
