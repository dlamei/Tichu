#include "renderer.h"
#include "gl_utils.h"
#include "imgui_build.h"
#include "Application.h"
#include <glad/glad.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <array>

#include "shader.glsl"

using index_t = uint32_t;

struct RenderContext {

    static constexpr uint32_t MAX_VERTICES = 4 * 1000;
    static constexpr uint32_t MAX_INDICES = 6 * 1000;
    static constexpr uint32_t MAX_TEXTURE_SLOT = 16;

    FrameBuffer frame_buffer{};
    Shader shader{};
    Buffer vertex_buffer{};
    Buffer index_buffer{};
    Texture white_texture{};

    std::array<Vertex, MAX_VERTICES> vertices{};
    std::array<index_t, MAX_INDICES> indices{};
    std::array<Texture, MAX_INDICES> textures{};
    int texture_index{1};

    Vertex *vertex_ptr{nullptr};
    index_t *index_ptr{nullptr};

    glm::mat4 camera{};

    uint32_t vertex_count{};
    uint32_t index_count{};

    Renderer::RectMode rect_mode = Renderer::RectMode::CORNER;
};

static RenderContext s_render_cntxt = {};

namespace Renderer {

    void reset() {
        s_render_cntxt.vertex_count = 0;
        s_render_cntxt.index_count = 0;

        s_render_cntxt.vertex_ptr = s_render_cntxt.vertices.data();
        s_render_cntxt.index_ptr = s_render_cntxt.indices.data();

        s_render_cntxt.texture_index = 1;
    }

    void init() {

        const auto texture = Texture::empty(100, 100);
        s_render_cntxt.frame_buffer = FrameBuffer::from_texture(texture);

        s_render_cntxt.shader = Shader::from_src(VERTEX_SRC, FRAGMENT_SRC);
        s_render_cntxt.camera = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -20.f, 20.f);

        s_render_cntxt.vertex_buffer = Buffer::vertex<Vertex>(nullptr, RenderContext::MAX_VERTICES);
        s_render_cntxt.index_buffer = Buffer::index32(nullptr, RenderContext::MAX_INDICES);

        auto white = RGBA{255};
        s_render_cntxt.white_texture = Texture::empty(1, 1);
        s_render_cntxt.white_texture.fill(&white, sizeof(RGBA));
        s_render_cntxt.textures[0] = s_render_cntxt.white_texture;

        int tex_slots[RenderContext::MAX_TEXTURE_SLOT];
        for (int i = 0; i < RenderContext::MAX_TEXTURE_SLOT; i++) tex_slots[i] = i;
        s_render_cntxt.shader.set_int_arr("u_texture_slots", tex_slots, RenderContext::MAX_TEXTURE_SLOT);

        reset();
    }

    int push_texture(const Texture &tex) {
        if (!tex.is_init()) {
            WARN("called push_texture with uninitialized texture");
            return 0;
        }

        for (int i = 0; i < s_render_cntxt.texture_index; i++) {
            if (s_render_cntxt.textures.at(i).native_texture() == tex.native_texture()) return i;
        }

        s_render_cntxt.textures.at(s_render_cntxt.texture_index) = tex;
        return s_render_cntxt.texture_index++;
    }

    void push_vert(const Vertex &v) {
        *s_render_cntxt.vertex_ptr = v;
        s_render_cntxt.vertex_ptr++;
    }

    void push_index(index_t i) {
        *s_render_cntxt.index_ptr = s_render_cntxt.vertex_count + i;
        s_render_cntxt.index_ptr++;
    }

    void rect_impl(const glm::vec2 &_pos, const glm::vec2 &size, RGBA tint, const Texture &texture, float rotation) {
        glm::vec3 pos{};
        switch (s_render_cntxt.rect_mode) {
            case RectMode::CORNER:
                pos = glm::vec3(_pos, 0);
                break;
            case RectMode::CENTER:
                pos = glm::vec3(_pos - size / 2.f, 0);
                break;
        }

        const uint32_t vert_count = 4;
        const uint32_t indx_count = 6;

        if (s_render_cntxt.vertex_count + vert_count >= RenderContext::MAX_VERTICES) flush();
        if (s_render_cntxt.index_count + indx_count >= RenderContext::MAX_INDICES) flush();
        if (s_render_cntxt.texture_index + 1 >= RenderContext::MAX_TEXTURE_SLOT) flush();

        auto tex_id = (float) push_texture(texture);

        // for now only rotate around center
        auto t1 = glm::translate(pos + glm::vec3(0));
        auto t2 = glm::translate(-(pos + glm::vec3(0)));
        auto scale = glm::scale(glm::vec3(size, 0));
        auto rotate = glm::rotate(glm::mat4(1.0f), rotation, {0.f, 0.f, 1.f});
        glm::mat4 transform = t1 * rotate * scale * t2;


        Vertex v{};
        v.col = tint.normalized();
        v.tex_id = tex_id;

        v.pos = transform * glm::vec4(pos, 1);
        v.uv = {0, 0};
        push_vert(v);

        v.pos = transform * glm::vec4(pos.x + 1, pos.y, 0, 1);
        v.uv = {1, 0};
        push_vert(v);

        v.pos = transform * glm::vec4(pos.x + 1, pos.y + 1, 0, 1);
        v.uv = {1, 1};
        push_vert(v);

        v.pos = transform * glm::vec4(pos.x, pos.y + 1, 0, 1);
        v.uv = {0, 1};
        push_vert(v);

        push_index(0);
        push_index(1);
        push_index(2);
        push_index(2);
        push_index(3);
        push_index(0);

        s_render_cntxt.vertex_count += vert_count;
        s_render_cntxt.index_count += indx_count;
    }

    void rect(const glm::vec2 &pos, const glm::vec2 &size, RGBA tint, const Texture &texture) {
        rect_impl(pos, size, tint, texture, 0.f);
    }

    void rect(const glm::vec2 &pos, const glm::vec2 &size, RGBA color) {
        rect(pos, size, color, s_render_cntxt.white_texture);
    }

    void rect(const glm::vec2 &pos, const glm::vec2 &size, const Texture &texture) {
        rect(pos, size, {255, 255, 255, 255}, texture);
    }


    void draw_tri(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, RGBA color) {
        const uint32_t vert_count = 3;
        const uint32_t indx_count = 3;

        if (s_render_cntxt.vertex_count + vert_count >= RenderContext::MAX_VERTICES) flush();
        if (s_render_cntxt.index_count + indx_count >= RenderContext::MAX_INDICES) flush();

        Vertex v{};
        v.col = color.normalized();
        v.uv = {0, 0};
        v.tex_id = 0;

        v.pos = glm::vec3(p1, 0);
        push_vert(v);
        v.pos = glm::vec3(p2, 0);
        push_vert(v);
        v.pos = glm::vec3(p3, 0);
        push_vert(v);

        push_index(0);
        push_index(1);
        push_index(2);

        s_render_cntxt.vertex_count += vert_count;
        s_render_cntxt.index_count += indx_count;
    }

    void flush() {
        s_render_cntxt.vertex_buffer.set_data(s_render_cntxt.vertices.data(),
                                              s_render_cntxt.vertex_count * sizeof(Vertex));
        s_render_cntxt.index_buffer.set_data(s_render_cntxt.indices.data(),
                                             s_render_cntxt.index_count * sizeof(index_t));
        s_render_cntxt.shader.set_mat4("u_camera", s_render_cntxt.camera);

        s_render_cntxt.shader.bind();
        s_render_cntxt.vertex_buffer.bind();
        s_render_cntxt.index_buffer.bind();
        Vertex::bind_layout();

        for (int i = 0; i < s_render_cntxt.texture_index; i++) {
            s_render_cntxt.textures.at(i).bind(i);
        }

        gl_utils::draw_indexed(s_render_cntxt.index_count);

        reset();
    }

    const FrameBuffer &get_frame_buffer() {
        return s_render_cntxt.frame_buffer;
    }

    void clear(RGBA clear_color) {
        auto col = clear_color.normalized();
        glClearColor(col.r, col.g, col.b, col.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void bind_frame_buffer() {
        s_render_cntxt.frame_buffer.bind();
    }

    void unbind_frame_buffer() {
        FrameBuffer::unbind();
    }

    void resize_frame_buffer() {
        auto size = Application::get_viewport_size();
        if (s_render_cntxt.frame_buffer.get_size() != size) {
            auto texture = Texture::empty(size.x, size.y);
            s_render_cntxt.frame_buffer = FrameBuffer::from_texture(texture);
        }
    }

    void set_camera(float left, float right, float bottom, float top) {
        s_render_cntxt.camera = glm::ortho(left, right, bottom, top, -20.f, 20.f);
    }

    void set(RectMode mode) {
        s_render_cntxt.rect_mode = mode;
    }

    void rect(const glm::vec2 &pos, const glm::vec2 &size, RGBA color, float rotation) {
        rect_impl(pos, size, color, s_render_cntxt.white_texture, rotation);
    }

}