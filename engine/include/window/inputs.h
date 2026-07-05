#pragma once
#include "common/event.h"
#include <GLFW/glfw3.h>

namespace flow {
    struct windowInputs {
        std::bitset<GLFW_MOUSE_BUTTON_LAST> Mouse;
        std::bitset<GLFW_KEY_LAST> Keys;
        double mouseX = 0.0;
        double mouseY = 0.0;
    };
}
