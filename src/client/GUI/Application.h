#ifndef TICHU_APPLICATION_H
#define TICHU_APPLICATION_H

#include <string>
#include <memory>
#include <glm/glm.hpp>

class Window;

struct ApplicationCreateInfo {
    std::string title = "Application";
    uint32_t width { 100 }, height { 100 };
};

class Application {
public:

    explicit Application(const ApplicationCreateInfo &info);
    ~Application();

    void run();

    [[nodiscard]] static const glm::uvec2 &get_viewport_size() { return get_instance()->_viewport_size; }
    [[nodiscard]] static const Application *get_instance();

private:

    void build_dock_frame();

    std::unique_ptr<Window> _window;
    glm::uvec2 _window_size {}, _viewport_size {};
    uint64_t _frame_count {0};

    static Application *s_instance;
};


#endif //TICHU_APPLICATION_H
