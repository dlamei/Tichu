#ifndef TICHU_RENDERER_H
#define TICHU_RENDERER_H

#include "gl_utils.h"


// provides some basic functions for simple immediate mode rendering
namespace Renderer {
    // initialize the renderer
    void init();
    void flush();

    // how to handle rectangle position
    enum class RectMode {
        // rectangle position defined by the top left corner (default)
        CORNER,
        // rectangle position defined by the center of the rectangle
        CENTER,
    };

    enum class RotateMode {
        CORNER,
        CENTER,
    };

    void rect(const glm::vec2 &pos, const glm::vec2 &size, RGBA color);
    void rect(const glm::vec2 &pos, const glm::vec2 &size, const Texture &texture);
    void rect(const glm::vec2 &pos, const glm::vec2 &size, RGBA tint, const Texture &texture);
    void rect(const glm::vec2 &pos, const glm::vec2 &size, RGBA color, float rotation);

    void rect_impl(const glm::vec2 &pos, const glm::vec2 &size, RGBA tint, const Texture &texture, float rotation);

    void draw_tri(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, RGBA color);

    void set(RectMode mode);
    void set(RotateMode mode);

    // clear the screen with the given color
    void clear(RGBA clear_color);

    // resize the framebuffer to the current size of the viewport
    void resize_frame_buffer();

    // bind the main framebuffer
    void bind_frame_buffer();

    // unbind framebuffer
    void unbind_frame_buffer();

    // get the main framebuffer
    const FrameBuffer &get_frame_buffer();

    void set_camera(float left, float right, float bottom, float top);

}

#endif //TICHU_RENDERER_H
