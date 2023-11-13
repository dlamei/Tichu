//
// light abstraction over glfw
//

#ifndef TICHU_WINDOW_H
#define TICHU_WINDOW_H

#include <string>
#include <optional>
#include <vector>

struct GLFWwindow;

// helper struct for creating a window
struct WindowCreateInfo {
    std::string title{ "Window Title" };
    uint32_t width { 100 }, height { 100 };
    std::optional<std::vector<uint32_t>> icon{};
};


class Window {
public:

    explicit Window(const WindowCreateInfo &info);
    void destroy();

    // updates the window
    void on_update();

    double get_time();
    // return true if the window was closed externally
    [[nodiscard]] bool should_close() const;

    void set_vsync(bool enable = true);

    [[nodiscard]] std::pair<uint32_t, uint32_t> get_window_size() const;
    [[nodiscard]] std::pair<float, float> get_mouse_pos() const;
    [[nodiscard]] std::pair<float, float> get_window_pos() const;

    [[nodiscard]] GLFWwindow *get_native_window() const { return _window; }

private:

    void init(const WindowCreateInfo &info);

    GLFWwindow *_window{};
    uint64_t _frame_count {0};
};


#endif //TICHU_WINDOW_H
