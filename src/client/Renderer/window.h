/*! \class Window
    \brief Provides interface to interact with and manage a window

 Uses graphics framework library "GLFW" with light abstractions.
*/

#ifndef TICHU_WINDOW_H
#define TICHU_WINDOW_H

#include <string>
#include <optional>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include <utility>
#include "key_codes.h"

struct GLFWwindow;

//! Helper struct for creating a window (object of class Window)
struct WindowCreateInfo {
    std::string title{"Window Title"};
    glm::uvec2 size{};
    std::optional<glm::ivec2> position{};
    bool vsync = true;
    std::optional<std::vector<uint32_t>> icon{};
};


class Window {
public:

    explicit Window(const WindowCreateInfo &info);

    void destroy();

    //! Updates the window
    void on_update();

    //! Checks if given key is pressed
    [[nodiscard]] bool is_key_pressed(KeyCode key) const;

    //! Checks if given mouse button is pressed
    [[nodiscard]] bool is_mouse_pressed(KeyCode mouse_button = KeyCode::MOUSE_BUTTON_1) const;
    [[nodiscard]] bool is_mouse_released(KeyCode mouse_button = KeyCode::MOUSE_BUTTON_1) const;

    double get_time();

    //! Returns true if the window was closed externally
    [[nodiscard]] bool should_close() const;

    void set_vsync(bool enable = true);

    [[nodiscard]] std::pair<uint32_t, uint32_t> get_window_size() const;

    [[nodiscard]] std::pair<float, float> get_mouse_pos() const;

    [[nodiscard]] std::pair<float, float> get_window_pos() const;

    [[nodiscard]] GLFWwindow *get_native_window() const { return _window; }

private:

    void init(const WindowCreateInfo &info);

    GLFWwindow *_window{};
    uint64_t _frame_count{0};
};


#endif //TICHU_WINDOW_H
