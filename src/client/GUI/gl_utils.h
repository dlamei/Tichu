#ifndef TICHU_GL_UTILS_H
#define TICHU_GL_UTILS_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <optional>
#include <filesystem>

struct Vertex {
    glm::vec3 pos;
    //glm::vec2 uv;

    static void bind_layout();
};

namespace gl_utils {

    void init_opengl();
    void resize_viewport(uint32_t width, uint32_t height);
}

class Texture {
public:

    ~Texture();

    Texture() = default;

    void bind();
    static void unbind();

    static Texture load(const std::filesystem::path &path);
    static Texture empty(uint32_t width, uint32_t height);

    [[nodiscard]] uint32_t width() const { return _width; }
    [[nodiscard]] uint32_t height() const { return _height; }
    [[nodiscard]] uint32_t native_texture() const { return *_gl_texture.value(); }

private:
    Texture(uint32_t width, uint32_t height, uint32_t n_channels, uint32_t gl_texture);

    std::optional<std::shared_ptr<uint32_t>> _gl_texture{};
    uint32_t _width{}, _height{}, _n_channels{};
};

class Shader {
public:

    Shader() = default;
    ~Shader();

    void bind();
    static void unbind();

    static Shader from_src(const char *vertex_src, const char *fragment_src);

private:
    explicit Shader(uint32_t gl_shader);

    std::optional<std::shared_ptr<uint32_t>> _gl_shader{};
};

enum class BufferType {
    INDEX,
    VERTEX,
    NONE,
};

class Buffer {
public:

    Buffer() = default;
    ~Buffer();

    [[nodiscard]] uint32_t size() const { return _size; }
    [[nodiscard]] uint32_t stride() const { return _stride; }
    [[nodiscard]] BufferType type() const { return _typ; }
    [[nodiscard]] uint32_t native_buffer() const { return *_gl_buffer.value(); }

    void bind() const;
    static void unbind(BufferType typ);

    static Buffer vertex(void *data, uint32_t size, uint32_t stride);
    static Buffer index(void *data, uint32_t size, uint32_t stride);

    template<typename VERTEX>
    static Buffer vertex(VERTEX *data, uint32_t count) {
        return vertex(data, count * sizeof(VERTEX), sizeof(VERTEX));
    }
private:

    Buffer(BufferType typ, uint32_t size, uint32_t stride, uint32_t gl_buffer);

    std::optional<std::shared_ptr<uint32_t>> _gl_buffer{};
    uint32_t _size{}, _stride{};
    BufferType _typ { BufferType::NONE };
};

class FrameBuffer {
public:

    FrameBuffer() = default;
    ~FrameBuffer();

    static FrameBuffer from_texture(const Texture& texture);

    void bind();
    static void unbind();

    [[nodiscard]] const Texture &get_attachment(uint32_t indx) const { return _attachments[indx]; };
    [[nodiscard]] glm::uvec2 get_size() const { return { _width, _height }; }

private:

    explicit FrameBuffer(uint32_t width, uint32_t height, uint32_t gl_frame_buffer);

    uint32_t _width{}, _height{};
    std::optional<std::shared_ptr<uint32_t>> _gl_frame_buffer{};
    std::vector<Texture> _attachments{};
};

void draw_impl(const Buffer &buffer);

template<typename VERTEX>
void draw(const Buffer &buffer) {
    buffer.bind();
    VERTEX::bind_layout();
    draw_impl(buffer);
}

#endif //TICHU_GL_UTILS_H
