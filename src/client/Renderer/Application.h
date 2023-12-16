#ifndef TICHU_APPLICATION_H
#define TICHU_APPLICATION_H

#include "renderer.h"
#include "KeyCodes.h"
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../../common/utils.h"

class Window;

// helper class for time
class TimeStep {
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

    virtual ~Layer() = default;

    /**
     * called when pushing the layer to the app
     */ 
    virtual void on_attach() {};

    /**
     * called in the Application destructor
     */ 
    virtual void on_detach() {};

    /**
     * can help separating gui and render code
    */ 
    virtual void on_update(TimeStep ts) {};

    virtual void on_imgui() {};
};


// helper struct for creating an app
struct ApplicationCreateInfo {
    std::string title = "Application";
    glm::uvec2 size {100, 100};
    std::optional<glm::ivec2> position {};
    std::string player_name{};
    bool auto_connect{false};
    //uint32_t width{100}, height{100};
};

// only single instance allowed
class Application {
public:

    explicit Application(const ApplicationCreateInfo &info);

    ~Application();

    // runs the app
    void run();

    // used to update the application when e.g the window is being resized
    static void update_frame();

    void push_layer(const std::shared_ptr<Layer> &layer);

    // returns the aspect ratio of the viewport
    [[nodiscard]] static float get_aspect_ratio() {
        const auto size = get_viewport_size();
        return (float) size.x / (float) size.y;
    }

    // returns the size of the viewport window
    [[nodiscard]] static const glm::uvec2 &get_viewport_size() { return get_instance()->_viewport_size; }
    [[nodiscard]] static const glm::uvec2 &get_viewport_pos() { return get_instance()->_viewport_pos;}

    // returns the size of the window
    [[nodiscard]] static const glm::uvec2 &get_window_size() { return get_instance()->_window_size; }

    [[nodiscard]] static glm::vec2 get_mouse_pos();

    // returns the application instance
    [[nodiscard]] static const Application *get_instance();

    [[nodiscard]] static const ApplicationCreateInfo &get_info() { return get_instance()->_info; };

private:

    void build_dock_frame();

    void update();

    ApplicationCreateInfo _info{};
    std::unique_ptr<Window> _window;
    glm::uvec2 _window_size{}, _viewport_size{}, _viewport_pos{};
    uint64_t _frame_count{0};
    float _last_frame_time{0};

    std::vector<std::shared_ptr<Layer>> _layers;

    static Application *s_instance;
};


#endif //TICHU_APPLICATION_H
