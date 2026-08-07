#include <flow.h>

int main(int32_t argc, char* argv[]) {
    using namespace flow;

    std::string projectPath;
    for (int32_t i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--project" && i + 1 < argc) projectPath = argv[++i];
    }

    auto app = new Application(engineMode::STANDALONE_GAME, projectPath);
    app->runContext();
    FLOW_DELETE(app);
    return 0;
}
