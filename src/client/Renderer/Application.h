/*! \class Application
    \brief Generates and manages the application framework.

 This is the core of the application framework, managing window creation, layer management, and providing utility functions
 to access information about the application state. \n
 Only a single instance is allowed.
*/

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

//! helper class for time measurement, representing the duration of time elapsed
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


//! Interface for running custom code in the application
class Layer {
public:

    virtual ~Layer() = default;


    //! Called when pushing the layer to the app
    virtual void on_attach() {};


    //! Called in the Application destructor
    virtual void on_detach() {};

<<<<<<< a321f63aa44e461949e05575aaf5c2b245178c4f
    /**
     * can help separating gui and render code
     */ 
=======

    //! Can help separating gui and render code
>>>>>>> 62dd44ee959e832e835f9f3015552842fa7cd7f3
    virtual void on_update(TimeStep ts) {};

    virtual void on_imgui() {};
};


//! Helper struct for creating an application
struct ApplicationCreateInfo {
    std::string title = "Application";
    glm::uvec2 size {1500, 1000};
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

<<<<<<< a321f63aa44e461949e05575aaf5c2b245178c4f
    /*
     * runs the app
    */
=======
    //! Runs the applicaton
>>>>>>> 62dd44ee959e832e835f9f3015552842fa7cd7f3
    void run();

    //! Updates the application when the window is being resized
    static void update_frame();

    void push_layer(const std::shared_ptr<Layer> &layer);

    //! Returns the aspect ratio of the viewport
    [[nodiscard]] static float get_aspect_ratio() {
        const auto size = get_viewport_size();
        return (float) size.x / (float) size.y;
    }

<<<<<<< a321f63aa44e461949e05575aaf5c2b245178c4f
    /**
     * returns the size of the viewport window
     */
    [[nodiscard]] static const glm::uvec2 &get_viewport_size() { return get_instance()->_viewport_size; }
    [[nodiscard]] static const glm::uvec2 &get_viewport_pos() { return get_instance()->_viewport_pos;}

    /**
     * returns the size of the window
     */ 
=======
    //! Returns the size of the viewport window
    [[nodiscard]] static const glm::uvec2 &get_viewport_size() { return get_instance()->_viewport_size; }
    [[nodiscard]] static const glm::uvec2 &get_viewport_pos() { return get_instance()->_viewport_pos;}

    //! Returns the size of the window
>>>>>>> 62dd44ee959e832e835f9f3015552842fa7cd7f3
    [[nodiscard]] static const glm::uvec2 &get_window_size() { return get_instance()->_window_size; }

    [[nodiscard]] static glm::vec2 get_mouse_pos();

<<<<<<< a321f63aa44e461949e05575aaf5c2b245178c4f
    /*
     * returns the application instance
     */
=======
    //! Returns the application instance
>>>>>>> 62dd44ee959e832e835f9f3015552842fa7cd7f3
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
