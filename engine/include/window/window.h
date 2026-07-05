#pragma once
#include "inputs.h"
#include "events.h"

namespace flow {
    struct appWindow {

        FLOW_INLINE appWindow(eventDispatcher* dispatcher, int32_t width, int32_t height,
            const char* title) : m_Dispatcher(dispatcher) {
            if (glfwInit() != GLFW_TRUE) {
                FLOW_CRITICAL("Failed to initialize GLFW");
                exit(EXIT_FAILURE);
            }
            glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_TRUE);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

            auto display = glfwGetVideoMode(glfwGetPrimaryMonitor());

            glfwWindowHint(GLFW_REFRESH_RATE, display->refreshRate);
            glfwWindowHint(GLFW_GREEN_BITS, display->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, display->blueBits);
            glfwWindowHint(GLFW_RED_BITS, display->redBits);
            glfwWindowHint(GLFW_SAMPLES, 4);

            glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

            m_Handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
            if (m_Handle == nullptr) {
                FLOW_CRITICAL("Failed to create window");
                exit(EXIT_FAILURE);
            }

            glfwSetWindowUserPointer(m_Handle, this);
            glfwSetWindowMaximizeCallback(m_Handle, onMaximize);
            glfwSetFramebufferSizeCallback(m_Handle, onResize);
            glfwSetWindowIconifyCallback(m_Handle, onIconify);
            glfwSetMouseButtonCallback(m_Handle, onMouse);
            glfwSetWindowCloseCallback(m_Handle, onClose);
            glfwSetCursorPosCallback(m_Handle, onMotion);
            glfwSetScrollCallback(m_Handle, onWheel);
            glfwSetKeyCallback(m_Handle, onKey);
            glfwSetErrorCallback(onError);


            glfwMakeContextCurrent(m_Handle);
            glfwSwapInterval(1);
        }

        FLOW_INLINE ~appWindow() {
            glfwDestroyWindow(m_Handle);
            glfwTerminate();
        }

        FLOW_INLINE GLFWwindow* getHandle() {
            return m_Handle;
        }

        FLOW_INLINE bool pollEvents() {
            glfwPollEvents();
            m_Dispatcher->pollEvents();
            glfwSwapBuffers(m_Handle);
            return !glfwWindowShouldClose(m_Handle);
        }

        FLOW_INLINE bool isKey(int32_t key) {
            if (key >= 0 && key <= GLFW_KEY_LAST){
                return m_Inputs.Keys.test(key);
            }
            return false;
        }

        FLOW_INLINE bool isMouse(int32_t button) {
            if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST) {
                return m_Inputs.Mouse.test(button);
            }
            return false;
        }

        FLOW_INLINE static appWindow* getUserData(GLFWwindow* window) {
            return static_cast<appWindow*>(glfwGetWindowUserPointer(window));
        }

        FLOW_INLINE static void onKey(GLFWwindow* window, int32_t key, int32_t, int32_t action, int32_t) {
            appWindow* self = getUserData(window);
            if (key >= 0 && key <= GLFW_KEY_LAST) {
                switch (action) {
                    case GLFW_RELEASE:
                        self->m_Dispatcher->postEvent<keyReleaseEvent>(key);
                        self->m_Inputs.Keys.reset(key);
                        break;

                    case GLFW_PRESS:
                        self->m_Dispatcher->postEvent<keyPressEvent>(key);
                        self->m_Inputs.Keys.set(key);
                        break;
                }
                return;
            }
            FLOW_ERROR("Invalid Key Code Detected: [{}]", key);
        }

        FLOW_INLINE static void onMouse(GLFWwindow* window, int button, int action, int) {
            appWindow* self = getUserData(window);
            if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST) {
                switch (action) {
                    case GLFW_PRESS:
                        self->m_Dispatcher->postEvent<mouseDownEvent>(button);
                        self->m_Inputs.Mouse.set(button);
                        break;

                    case GLFW_RELEASE:
                        self->m_Dispatcher->postEvent<mouseReleaseEvent>(button);
                        self->m_Inputs.Mouse.reset(button);
                        break;
                }
                return;
            }
            FLOW_ERROR("Invalid Mouse Button Detected: [{}]", button);
        }

        FLOW_INLINE static void onResize(GLFWwindow* window, int32_t width, int32_t height) {
            getUserData(window)->m_Dispatcher->postEvent<windowResizeEvent>(width, height);
        }

        FLOW_INLINE static void onMotion(GLFWwindow* window, double xpos, double ypos) {
            appWindow* self = getUserData(window);
            int x = static_cast<int>(xpos);
            int y = static_cast<int>(ypos);
            self->m_Dispatcher->postEvent<mouseMotionEvent>(x, y);
            if (self->m_Inputs.Mouse.test(GLFW_MOUSE_BUTTON_LEFT)){
                self->m_Dispatcher->postEvent<mouseDragEvent>(
                    self->m_Inputs.mouseX - x, self->m_Inputs.mouseY - y);
            }
            self->m_Inputs.mouseX = x;
            self->m_Inputs.mouseY = y;
        }

        FLOW_INLINE static void onWheel(GLFWwindow* window, double x, double y) {
            getUserData(window)->m_Dispatcher->postEvent<mouseWheelEvent>(x, y);
        }

        FLOW_INLINE static void onMaximize(GLFWwindow* window, int32_t action) {
            if (action) {
                getUserData(window)->m_Dispatcher->postEvent<windowMaximizeEvent>();
            }
            else {
                getUserData(window)->m_Dispatcher->postEvent<windowRestoreEvent>();
            }
        }

        FLOW_INLINE static void onIconify(GLFWwindow* window, int32_t action) {
            if (action) {
                getUserData(window)->m_Dispatcher->postEvent<windowIconifyEvent>();
            }
            else {
                getUserData(window)->m_Dispatcher->postEvent<windowRestoreEvent>();
            }
        }

        FLOW_INLINE static void onError(int32_t code, const char* msg) {
            FLOW_ERROR("[GLFW]: [{}] {}", code, msg);
        }

        FLOW_INLINE static void onClose(GLFWwindow* window) {
            getUserData(window)->m_Dispatcher->postEvent<windowCloseEvent>();
        }

    private:
        eventDispatcher* m_Dispatcher;
        windowInputs m_Inputs;
        GLFWwindow* m_Handle;
    };
}
