#pragma once
#include "common/event.h"

namespace flow {
    // window events
    struct windowMaximizeEvent {};
    struct windowIconifyEvent {};
    struct windowRestoreEvent {};
    struct windowCloseEvent {};

    struct windowResizeEvent {
        windowResizeEvent(int32_t w, int32_t h) : width(w), height(h) {}
        int32_t width = 0;
        int32_t height = 0;
    };

    // key events
    struct keyPressEvent {
        keyPressEvent(int32_t k) : key(k) {}
        int32_t key = -1;
    };

    struct keyReleaseEvent {
        keyReleaseEvent(int32_t k) : key(k) {}
        int32_t key = -1;
    };

    struct keyRepeatEvent {
        keyRepeatEvent(int32_t k) : key(k) {}
        int32_t key = -1;
    };


    // mouse events
    struct mouseReleaseEvent {
        mouseReleaseEvent(int32_t b) : button(b) {}
        int32_t button = -1;
    };

    struct mouseDownEvent {
        mouseDownEvent(int32_t b) : button(b) {}
        int32_t button = -1;
    };

    struct mouseDragEvent {
        mouseDragEvent(double dx, double dy) : deltaX(dx), deltaY(dy) {}
        double deltaX = 0;
        double deltaY = 0;
    };

    struct mouseMotionEvent {
        mouseMotionEvent(double x, double y) : targetX(x), targetY(y) {}
        double targetX = 0;
        double targetY = 0;
    };

    struct mouseWheelEvent {
        mouseWheelEvent(double sx, double sy) : scrollX(sx), scrollY(sy) {}
        double scrollX = 0;
        double scrollY = 0;
    };
}
