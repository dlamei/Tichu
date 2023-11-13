#include "renderer.h"
#include "gl_utils.h"
#include "imgui_build.h"
#include "Application.h"

#include <glm/gtx/transform.hpp>

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


const char *VERTEX_SRC = R"(
#version 330 core

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_uv;

uniform mat4 camera;

out vec2 uv;

void main() {
    gl_Position = camera * vec4(in_pos, 0.0, 1.0);
    uv = in_uv;
}
)";

const char *FRAGMENT_SRC = R"(
#version 330 core
out vec4 FragColor;

in vec2 uv;

void main() {
    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //FragColor = vec4(uv.x, uv.y, uv.x * uv.y, 1.0f);
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

using index_t = uint32_t;

struct RenderContext {
    FrameBuffer frame_buffer{};
    Shader shader{};
    std::vector<Vertex2D> vertices{};
    std::vector<index_t> indices{};
    glm::mat4 camera{};

    uint32_t vertex_count{};
    uint32_t index_count{};

    Buffer vertex_buffer{};
    Buffer index_buffer{};
};

static RenderContext s_RenderContext = {};

namespace Renderer {

    void init() {

        auto texture = Texture::empty(100, 100);
        s_RenderContext.frame_buffer = FrameBuffer::from_texture(texture);

        s_RenderContext.shader = Shader::from_src(VERTEX_SRC, FRAGMENT_SRC);
        s_RenderContext.camera = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -20.f, 20.f);
    }

    void push_vert(const Vertex2D &v) {
        s_RenderContext.vertices.push_back(v);
    }

    void push_index(index_t i) {
        auto index = s_RenderContext.vertex_count + i;
        s_RenderContext.indices.push_back(index);
    }

    void draw_rect(glm::vec2 pos, glm::vec2 size) {
        Vertex2D v{};

        v.pos = pos;
        v.uv = { 0, 0 };
        push_vert(v);

        v.pos = { pos.x + size.x, pos.y };
        v.uv = { 1, 0 };
        push_vert(v);

        v.pos = { pos.x + size.x, pos.y + size.y };
        v.uv = { 1, 1 };
        push_vert(v);

        v.pos = { pos.x, pos.y + size.y };
        v.uv = { 0, 1 };
        push_vert(v);

        push_index(0);
        push_index(1);
        push_index(2);
        push_index(2);
        push_index(3);
        push_index(0);

        s_RenderContext.vertex_count += 4;
        s_RenderContext.index_count += 6;
    }


    void draw_tri(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
        Vertex2D v{};
        v.uv = { 0, 0 };

        v.pos = p1;
        push_vert(v);
        v.pos = p2;
        push_vert(v);
        v.pos = p3;
        push_vert(v);

        push_index(0);
        push_index(1);
        push_index(2);

        s_RenderContext.vertex_count += 3;
        s_RenderContext.index_count += 3;
    }

    void reset() {
        s_RenderContext.vertex_count = 0;
        s_RenderContext.index_count = 0;

        s_RenderContext.vertices.clear();
        s_RenderContext.indices.clear();
    }

    void flush() {
        s_RenderContext.vertex_buffer = Buffer::vertex(s_RenderContext.vertices.data(), s_RenderContext.vertex_count);
        s_RenderContext.index_buffer = Buffer::index32(s_RenderContext.indices.data(), s_RenderContext.index_count);

        s_RenderContext.shader.bind();
        s_RenderContext.shader.set_mat4("camera", s_RenderContext.camera);

        s_RenderContext.vertex_buffer.bind();
        s_RenderContext.index_buffer.bind();
        Vertex2D::bind_layout();

        gl_utils::draw_indexed(s_RenderContext.index_count);

        reset();
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

    void set_camera(float left, float right, float bottom, float top) {
        s_RenderContext.camera = glm::ortho(left, right, bottom, top, -20.f, 20.f);
    }

}