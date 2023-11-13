//
// helper functions and classes for opengl
//



#ifndef TICHU_GL_UTILS_H
#define TICHU_GL_UTILS_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <optional>
#include <filesystem>

struct Vertex2D {
    glm::vec2 pos;
    glm::vec2 uv;

    static void bind_layout();
};

namespace gl_utils {

    void init_opengl();
    void resize_viewport(uint32_t width, uint32_t height);

    void draw(uint32_t vert_count);
    void draw_indexed(uint32_t index_count);
}

// opengl texture that internally handles creating & destroying memory
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

    // functions for setting shader unifroms
    void set_int(const std::string &name, int32_t value);
    void set_float(const std::string &name, float value);
    void set_mat4(const std::string &name, const glm::mat4 &mat);

private:
    explicit Shader(uint32_t gl_shader);


    std::optional<std::shared_ptr<uint32_t>> _gl_shader{};
};

enum class BufferType {
    // index buffer
    INDEX,
    // vertex buffer
    VERTEX,
    NONE,
};

class Buffer {
public:

    Buffer() = default;
    ~Buffer();

    // get the number of elements in the buffer
    [[nodiscard]] uint32_t count() const { return _size / _stride; }
    // get the size of the entire buffer in bytes
    [[nodiscard]] uint32_t size() const { return _size; }
    // get the size of a single element in bytes
    [[nodiscard]] uint32_t stride() const { return _stride; }
    // get the type of the buffer
    [[nodiscard]] BufferType type() const { return _typ; }
    [[nodiscard]] uint32_t native_buffer() const { return *_gl_buffer.value(); }

    void bind() const;
    static void unbind(BufferType typ);

    // vertex buffer constructor
    static Buffer vertex(void *data, uint32_t size, uint32_t stride);
    // index buffer constructor
    static Buffer index32(uint32_t *data, uint32_t size);

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

// opengl framebuffer abstraction. currently only allows for texture2D attachments
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

#endif //TICHU_GL_UTILS_H
