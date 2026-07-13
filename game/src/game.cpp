#include <flow.h>

int main(int32_t argc, char* argv[]) {
    using namespace flow;
    auto app = new Application();
    app->runContext();
    FLOW_DELETE(app);
    return 0;
}
