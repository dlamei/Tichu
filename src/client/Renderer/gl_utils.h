//
// helper functions and classes for opengl
//

#ifndef TICHU_GL_UTILS_H
#define TICHU_GL_UTILS_H

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <optional>
#include <filesystem>
#include "../../common/utils.h"

//class RGB;
class ImVec4;


// helper class for color
class RGBA {
public:
    RGBA();

    explicit RGBA(uint8_t value);

    RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    RGBA(uint8_t r, uint8_t g, uint8_t b);

    static RGBA from_norm(glm::vec3 val);
    static RGBA from_norm(glm::vec4 val);
    static RGBA from(const ImVec4 &vec);

    [[nodiscard]] inline uint8_t red() const;

    [[nodiscard]] inline uint8_t green() const;

    [[nodiscard]] inline uint8_t blue() const;

    [[nodiscard]] inline uint8_t alpha() const;

    [[nodiscard]] glm::vec4 normalized() const;

    explicit operator uint32_t() const;
    //explicit operator RGB() const;

private:
    uint32_t m_Data;
};

inline bool operator==(const RGBA &c1, const RGBA &c2) {
    return (uint32_t) c1 == (uint32_t) c2;
}

inline bool operator!=(const RGBA &c1, const RGBA &c2) {
    return (uint32_t) c1 != (uint32_t) c2;
}

std::ostream &operator<<(std::ostream &os, const RGBA &c);

// helper class for color when alpha is assumed to be 255
// we can't just use RGBA because we want to be able to fill textures with these types, so RGB 3 bytes and RGBA 4 bytes
//class RGB {
//public:
//    RGB();
//    explicit RGB(uint8_t value);
//    RGB(uint8_t r, uint8_t g, uint8_t b);
//
//    static RGB from_norm(glm::vec3 val);
//
//    [[nodiscard]] inline uint8_t red() const { return _red; }
//    [[nodiscard]] inline uint8_t green() const { return _green; }
//    [[nodiscard]] inline uint8_t blue() const { return _blue; }
//
//    [[nodiscard]] glm::vec3 normalized() const;
//
//    explicit operator RGBA() const;
//
//private:
//    uint8_t _blue;
//    uint8_t _green;
//    uint8_t _red;
//};
//
//inline bool operator==(const RGB &c1, const RGB &c2) {
//    return (RGBA)c1 == (RGBA)c2;
//}
//
//inline bool operator!=(const RGB &c1, const RGB &c2) {
//    return (RGBA)c1 != (RGBA)c2;
//}
//
//std::ostream &operator<<(std::ostream &os, const RGB &c);

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec4 col;
    float tex_id;

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

    void fill(void *data, size_t size);

    void bind(int i = 0);

    static void unbind(int i = 0);

    static Texture load(const std::filesystem::path &path);

    static Texture empty(uint32_t width, uint32_t height);

    [[nodiscard]] uint32_t width() const { return _width; }
    [[nodiscard]] uint32_t height() const { return _height; }
    [[nodiscard]] uint32_t native_texture() const { ASSERT(is_init(), "texture not initialized"); return *_gl_texture.value(); }
    [[nodiscard]] bool is_init() const { return _gl_texture.has_value(); }

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

    void set_int_arr(const std::string &name, int32_t *data, size_t count);

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
    static Buffer vertex(void *data, uint32_t count, uint32_t stride);

    // index buffer constructor
    static Buffer index32(uint32_t *data, uint32_t count);

    void set_data(void *data, size_t size);

    template<typename VERTEX>
    static Buffer vertex(VERTEX *data, uint32_t count) {
        return vertex(data, count * sizeof(VERTEX), sizeof(VERTEX));
    }

private:

    Buffer(BufferType typ, uint32_t size, uint32_t stride, uint32_t gl_buffer);

    std::optional<std::shared_ptr<uint32_t>> _gl_buffer{};
    uint32_t _size{}, _stride{};
    BufferType _typ{BufferType::NONE};
};

// opengl framebuffer abstraction. currently only allows for texture2D attachments
class FrameBuffer {
public:

    FrameBuffer() = default;

    ~FrameBuffer();

    static FrameBuffer from_texture(const Texture &texture);

    void bind();

    static void unbind();

    [[nodiscard]] const Texture &get_attachment(uint32_t indx) const { return _attachments[indx]; };

    [[nodiscard]] glm::uvec2 get_size() const { return {_width, _height}; }

private:

    explicit FrameBuffer(uint32_t width, uint32_t height, uint32_t gl_frame_buffer);

    uint32_t _width{}, _height{};
    std::optional<std::shared_ptr<uint32_t>> _gl_frame_buffer{};
    std::vector<Texture> _attachments{};
};

#endif //TICHU_GL_UTILS_H
