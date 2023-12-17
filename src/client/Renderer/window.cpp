#include "window.h"
#include "gl_utils.h"
#include "imgui_build.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

//#include "../../../libs/glfw/src/internal.h"
#include "application.h"
#include "../../src/common/utils.h"

static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char *msg) {
    ERROR("GLFW Error ({0}): {1}", error, msg);
}


Window::Window(const WindowCreateInfo &info) {
    init(info);
}

void Window::init(const WindowCreateInfo &info) {


    if (!s_GLFWInitialized) {
        int success = glfwInit();
        ASSERT(success, "Could not initialize GLFW")
        glfwSetErrorCallback(GLFWErrorCallback);
        s_GLFWInitialized = true;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    _window = glfwCreateWindow((int) info.size.x, (int) info.size.y, info.title.c_str(),
                               nullptr, nullptr);

    ASSERT(_window, "Could not create window");

    glfwMakeContextCurrent(_window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    if (info.icon.has_value()) {
        auto logoSize = (double) info.icon.value().size();
        GLFWimage images{};
        images.width = (int) sqrt(logoSize);
        images.height = images.width;
        images.pixels = (uint8_t *) info.icon.value().data();
        glfwSetWindowIcon(_window, 1, &images);
    }

    if (info.position.has_value()) {
        auto pos = info.position.value();
        glfwSetWindowPos(_window, pos.x, pos.y);
    }

    {
        int w, h;
        glfwGetFramebufferSize(_window, &w, &h);
    }

    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowUserPointer(_window, this);
    set_vsync(info.vsync);

    glfwSetWindowSizeCallback(_window, [](GLFWwindow *window, int width, int height) {
        Window &data = *(Window *) glfwGetWindowUserPointer(window);

        Application::update_frame();
        glfwSwapBuffers(data.get_native_window());
    });

    gl_utils::init_opengl();
    ImGui::init_imgui(*this);
}

void Window::destroy() {
    glfwDestroyWindow(_window);
}

void Window::on_update() {
    glfwPollEvents();
    glfwSwapBuffers(_window);

}

bool Window::is_key_pressed(KeyCode key) const {
    const auto state = glfwGetKey(_window, (int) key);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Window::is_mouse_pressed(KeyCode mouse_button) const {
    const auto state = glfwGetMouseButton(_window, (int) mouse_button);
    return state == GLFW_PRESS;
}

bool Window::is_mouse_released(KeyCode mouse_button) const {
    const auto state = glfwGetMouseButton(_window, (int) mouse_button);
    return state == GLFW_RELEASE;
}

double Window::get_time() {
    return glfwGetTime();
}

bool Window::should_close() const {
    return glfwWindowShouldClose(_window);
}

void Window::set_vsync(bool enable) {
    glfwSwapInterval(enable);
}

std::pair<float, float> Window::get_mouse_pos() const {
    double mouse_x, mouse_y;
    glfwGetCursorPos(_window, &mouse_x, &mouse_y);
    return {(float) mouse_x, (float) mouse_y};
}

std::pair<float, float> Window::get_window_pos() const {
    int pos_x, pos_y;
    glfwGetWindowPos(_window, &pos_x, &pos_y);
    return {(float) pos_x, (float) pos_y};
}

std::pair<uint32_t, uint32_t> Window::get_window_size() const {
    int w, h;
    glfwGetFramebufferSize(_window, &w, &h);
    return {w, h};
}

