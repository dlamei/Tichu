#ifndef TICHU_APPLICATION_H
#define TICHU_APPLICATION_H

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../../common/logging.h"

class Window;

// helper class for time
class TimeStep
{
private:
    float m_Time;

public:
    explicit TimeStep(float time = 0.0f)
            : m_Time(time) {}

    explicit operator float() const { return m_Time; }

    [[nodiscard]] inline float seconds() const { return m_Time; }
    [[nodiscard]] inline float milliseconds() const { return m_Time * 1000.0f; }
};

// interface for running custom code in the application
class Layer {
public:

    // called when pushing the layer to the app
    virtual void on_attach() {};

    // called in the Application destructor
    virtual void on_detach() {};

    // can help separating gui and render code
    virtual void on_update(TimeStep ts) {};
    virtual void on_imgui() {};
};


// helper struct for creating an app
struct ApplicationCreateInfo {
    std::string title = "Application";
    uint32_t width { 100 }, height { 100 };
};

// only single instance allowed
class Application {
public:

    explicit Application(const ApplicationCreateInfo &info);
    ~Application();

    // runs the app
    void run();

    void push_layer(const std::shared_ptr<Layer> &layer);

    // returns the size of the viewport window
    [[nodiscard]] static const glm::uvec2 &get_viewport_size() { return get_instance()->_viewport_size; }
    // returns the size of the window
    [[nodiscard]] static const glm::uvec2 &get_window_size() { return get_instance()->_window_size; }
    // returns the application instance
    [[nodiscard]] static const Application *get_instance();

private:

    void build_dock_frame();
    void update();

    std::unique_ptr<Window> _window;
    glm::uvec2 _window_size {}, _viewport_size {};
    glm::ivec2 _window_mouse_pos {}, _viewport_mouse_pos {};
    uint64_t _frame_count {0};
    float _last_frame_time{0};

    std::vector<std::shared_ptr<Layer>> _layers;

    static Application *s_instance;
};


#endif //TICHU_APPLICATION_H
