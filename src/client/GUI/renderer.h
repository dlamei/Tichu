#ifndef TICHU_RENDERER_H
#define TICHU_RENDERER_H

#include "gl_utils.h"

class RGB;

class RGBA {
public:
    RGBA();
    explicit RGBA(uint8_t value);
    RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    RGBA(uint8_t r, uint8_t g, uint8_t b);

    static RGBA from_norm(glm::vec3 val);
    static RGBA from_norm(glm::vec4 val);

    [[nodiscard]] inline uint8_t red() const;
    [[nodiscard]] inline uint8_t green() const;
    [[nodiscard]] inline uint8_t blue() const;
    [[nodiscard]] inline uint8_t alpha() const;

    [[nodiscard]] glm::vec4 normalized() const;

    explicit operator uint32_t() const;
    explicit operator RGB() const;

private:
    uint32_t m_Data;
};

inline bool operator==(const RGBA &c1, const RGBA &c2) {
    return (uint32_t)c1 == (uint32_t)c2;
}

inline bool operator!=(const RGBA &c1, const RGBA &c2) {
    return (uint32_t)c1 != (uint32_t)c2;
}

std::ostream &operator<<(std::ostream &os, const RGBA &c);

class RGB {
public:
    RGB();
    explicit RGB(uint8_t value);
    RGB(uint8_t r, uint8_t g, uint8_t b);

    static RGB from_norm(glm::vec3 val);

    [[nodiscard]] inline uint8_t red() const { return m_Red; }
    [[nodiscard]] inline uint8_t green() const { return m_Green; }
    [[nodiscard]] inline uint8_t blue() const { return m_Blue; }

    [[nodiscard]] glm::vec3 normalized() const;

    explicit operator RGBA() const;

private:
    uint8_t m_Blue;
    uint8_t m_Green;
    uint8_t m_Red;
};

inline bool operator==(const RGB &c1, const RGB &c2) {
    return (RGBA)c1 == (RGBA)c2;
}

inline bool operator!=(const RGB &c1, const RGB &c2) {
    return (RGBA)c1 != (RGBA)c2;
}

std::ostream &operator<<(std::ostream &os, const RGB &c);

namespace Renderer {
    void init();
    void update();
    void clear(RGBA clear_color);

    void resize_frame_buffer();
    void bind_frame_buffer();
    void unbind_frame_buffer();
    const FrameBuffer &get_frame_buffer();

}

#endif //TICHU_RENDERER_H
