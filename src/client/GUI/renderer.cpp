#include "renderer.h"
#include "gl_utils.h"
#include "imgui_build.h"
#include "Application.h"

inline uint32_t to_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (a << 24) | (b << 16) | (g << 8) | r;
}

RGBA::RGBA()
        : m_Data(to_rgba(255, 255, 255, 255)) {};
RGBA::RGBA(uint8_t value)
        : m_Data(to_rgba(value, value, value, 255)) {};
RGBA::RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : m_Data(to_rgba(r, g, b, a)) {};
RGBA::RGBA(uint8_t r, uint8_t g, uint8_t b)
        : m_Data(to_rgba(r, g, b, 255)) {}

RGBA RGBA::from_norm(glm::vec3 val)
{
    auto r = static_cast<uint8_t>(val.r * 255);
    auto g = static_cast<uint8_t>(val.g * 255);
    auto b = static_cast<uint8_t>(val.b * 255);
    return {r, g, b};
}

RGBA RGBA::from_norm(glm::vec4 val)
{
    auto r = static_cast<uint8_t>(val.r * 255);
    auto g = static_cast<uint8_t>(val.g * 255);
    auto b = static_cast<uint8_t>(val.b * 255);
    auto a = static_cast<uint8_t>(val.a * 255);
    return {r, g, b, a};
}

inline uint8_t RGBA::red() const
{
    return m_Data & 0xff;
}

inline uint8_t RGBA::green() const
{
    return m_Data >> 8 & 0xff;
}

inline uint8_t RGBA::blue() const
{
    return m_Data >> 16 & 0xff;
}

inline uint8_t RGBA::alpha() const
{
    return m_Data >> 24 & 0xff;
}

glm::vec4 RGBA::normalized() const
{
    float r = (float)red() / 255.f;
    float g = (float)green() / 255.f;
    float b = (float)blue() / 255.f;
    float a = (float)alpha() / 255.f;

    return { r, g, b, a };
}

RGBA::operator uint32_t() const
{
    return m_Data;
}

RGBA::operator RGB() const
{
    return {red(), green(), blue()};
}

std::ostream &operator<<(std::ostream &os, const RGBA &c) {
    os << "{ r: " << (uint32_t)c.red() << ", g: " << (uint32_t)c.green() << ", b: "
       << (uint32_t)c.blue() << ", a: " << (uint32_t)c.alpha() << " }";
    return os;
}

RGB::RGB()
        : m_Red(255), m_Blue(255), m_Green(255) {}

RGB::RGB(uint8_t value)
        : m_Red(value), m_Blue(value), m_Green(value) {}

RGB::RGB(uint8_t r, uint8_t g, uint8_t b)
        : m_Red(r), m_Blue(g), m_Green(b) {}

RGB RGB::from_norm(glm::vec3 val)
{
    auto r = static_cast<uint8_t>(val.r * 255);
    auto g = static_cast<uint8_t>(val.g * 255);
    auto b = static_cast<uint8_t>(val.b * 255);
    return {r, g, b};
}

glm::vec3 RGB::normalized() const
{
    float r = (float)red() / 255.f;
    float g = (float)green() / 255.f;
    float b = (float)blue() / 255.f;

    return { r, g, b };
}

RGB::operator RGBA() const
{
    return {m_Red, m_Green, m_Blue};
}

std::ostream &operator<<(std::ostream &os, const RGB &c) {
    os << "{ r: " << (uint32_t)c.red() << ", g: " << (uint32_t)c.green() << ", b: "
       << (uint32_t)c.blue() << " }";
    return os;
}


struct RenderContext {
    FrameBuffer frame_buffer{};
    Shader shader;
    //uint32_t shader;
    Buffer vertex_buffer;
    //uint32_t vao;
};

static RenderContext s_RenderContext = {};

const char *VERTEX_SRC = R"(
#version 330 core

layout (location = 0) in vec3 pos;

void main() {
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}
)";

const char *FRAGMENT_SRC = R"(
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

namespace Renderer {

    void init() {

        auto texture = Texture::empty(100, 100);
        s_RenderContext.frame_buffer = FrameBuffer::from_texture(texture);

        Vertex vertices[] = {
                {{0.f, 0.f, 0.f}},
                {{100.f, 100.f, 0.f}},
                {{0, 400.f, 0.f}},
        };

        s_RenderContext.vertex_buffer = Buffer::vertex(vertices, 3);
        s_RenderContext.shader = Shader::from_src(VERTEX_SRC, FRAGMENT_SRC);
    }

    void update() {
        s_RenderContext.shader.bind();
        s_RenderContext.vertex_buffer.bind();
        draw<Vertex>(s_RenderContext.vertex_buffer);
    }

    const FrameBuffer &get_frame_buffer() {
        return s_RenderContext.frame_buffer;
    }

    void clear(RGBA clear_color) {
        auto col = clear_color.normalized();
        glClearColor(col.r, col.g, col.b, col.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void bind_frame_buffer() {
        s_RenderContext.frame_buffer.bind();
    }

    void unbind_frame_buffer() {
        FrameBuffer::unbind();
    }

    void resize_frame_buffer() {
        auto size = Application::get_viewport_size();
        auto dsize = size - s_RenderContext.frame_buffer.get_size();
        if (dsize.x * dsize.x + dsize.y * dsize.y > 100) {
            auto texture = Texture::empty(size.x, size.y);
            s_RenderContext.frame_buffer = FrameBuffer::from_texture(texture);
        }
    }


}