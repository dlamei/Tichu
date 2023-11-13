#include "gl_utils.h"
#include <stb_image.h>
#include <cstddef>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include "../../src/common/logging.h"

#define INTERNAL_FORMAT GL_RGBA8
#define EXTERNAL_FORMAT GL_RGBA

#define GL_VERTEX_BUFFER GL_ARRAY_BUFFER
#define GL_INDEX_BUFFER GL_ELEMENT_ARRAY_BUFFER

void Vertex2D::bind_layout() {
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void *) offsetof(Vertex2D, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void *) offsetof(Vertex2D, uv));
    glEnableVertexAttribArray(1);
}

namespace gl_utils {


    void init_opengl() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        uint32_t VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    }

    void resize_viewport(uint32_t width, uint32_t height) {
        glViewport(0, 0, width, height);
    }

    void draw_indexed(uint32_t index_count) {
        glDrawElements(GL_TRIANGLES, (int)index_count, GL_UNSIGNED_INT, nullptr);
    }

    void draw(uint32_t vert_count) {
        glDrawArrays(GL_TRIANGLES, 0, (int)vert_count);
    }

    void create_texture2d(uint32_t width, uint32_t height, uint32_t *id) {
        glGenTextures(1, id);
        glBindTexture(GL_TEXTURE_2D, *id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, width, height, 0, EXTERNAL_FORMAT, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void load_texture2d(const std::string &path, uint32_t *width, uint32_t *height, uint32_t *n_channels, uint32_t *id) {
        glGenTextures(1, id);
        glBindTexture(GL_TEXTURE_2D, *id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int w, h, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path.c_str(), &w, &h, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, w, h, 0, EXTERNAL_FORMAT, GL_UNSIGNED_BYTE, data);
        } else {
            WARN("Failed to load texture: {}", path);
        }
        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0);

        *width = (uint32_t)w;
        *height = (uint32_t)h;
        *n_channels = (uint32_t)nrChannels;
    }

    void create_buffer(GLenum type, uint32_t *id, void *data, uint32_t size) {
        glGenBuffers(1, id);
        glBindBuffer(type, *id);
        glBufferData(type, size, data, GL_STATIC_DRAW);
        glBindBuffer(type, 0);
    }

    void compile_shader_module(GLenum typ, const char *shader_src, uint32_t *id) {
        uint32_t module = glCreateShader(typ);
        glShaderSource(module, 1, &shader_src, NULL);
        glCompileShader(module);

        int success;
        char infoLog[1024];
        glGetShaderiv(module, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(module, 1024, NULL, infoLog);
            ERROR("SHADER_COMPILATION_ERROR:");
            ERROR("{}", infoLog);
        }

        *id = module;
    }

    void link_shader_modules(uint32_t vertex_module, uint32_t fragment_module, uint32_t *id) {
        uint32_t shader = glCreateProgram();
        glAttachShader(shader, vertex_module);
        glAttachShader(shader, fragment_module);
        glLinkProgram(shader);

        glDeleteShader(vertex_module);
        glDeleteShader(fragment_module);

        int success;
        char infoLog[1024];
        glGetShaderiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            ERROR("SHADER_LINKING_ERROR:");
            ERROR("{}", infoLog);
        }

        *id = shader;
    }
}

#define GET_ID(id, opt_shared_ptr)    \
if (!opt_shared_ptr.has_value()) {     \
    WARN("{} ({}): item was not initialized!", __FUNCTION__, #opt_shared_ptr); \
    return;                         \
}                                   \
auto id = *opt_shared_ptr.value();

Texture::~Texture() {
    if (_gl_texture.has_value() && _gl_texture.value().use_count() == 1) {
        glDeleteTextures(1, &*_gl_texture.value());
    }
}

Texture Texture::load(const std::filesystem::path &path) {
    uint32_t width, height, n_chanels, id;
    gl_utils::load_texture2d(path.string(), &width, &height, &n_chanels, &id);
    return Texture {width, height, n_chanels, id};
}

Texture::Texture(uint32_t width, uint32_t height, uint32_t n_channels, uint32_t gl_texture)
    : _height(height), _width(width), _n_channels(n_channels), _gl_texture(std::make_shared<uint32_t>(gl_texture))
{
}

void Texture::bind() {
    GET_ID(id, _gl_texture);
    glBindTexture(GL_TEXTURE_2D, *_gl_texture.value());
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture Texture::empty(uint32_t width, uint32_t height) {
    width = std::max(width, 1u);
    height = std::max(height, 1u);
    uint32_t id;
    gl_utils::create_texture2d(width, height, &id);
    return Texture { width, height, 1, id };
}

GLenum buffer_type_to_gl_enum(BufferType typ) {
    switch (typ) {
        case BufferType::INDEX:
            return GL_INDEX_BUFFER;
        case BufferType::VERTEX:
            return GL_VERTEX_BUFFER;
        default:
            ASSERT(false, "found unknown BufferType");
    }
}

Buffer::Buffer(BufferType typ, uint32_t size, uint32_t stride, uint32_t gl_buffer)
    : _typ(typ), _size(size), _stride(stride), _gl_buffer(std::make_shared<uint32_t>(gl_buffer))
{

}

Buffer::~Buffer() {
    if (_gl_buffer.has_value() && _gl_buffer.value().use_count() == 1) {
        glDeleteBuffers(1, &*_gl_buffer.value());
    }
}

Buffer Buffer::vertex(void *data, uint32_t size, uint32_t stride) {
    uint32_t id;

    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return Buffer { BufferType::VERTEX, size, stride, id};
}

Buffer Buffer::index32(uint32_t *data, uint32_t size) {
    uint32_t id;
    gl_utils::create_buffer(GL_INDEX_BUFFER, &id, data, size * sizeof(uint32_t));
    return Buffer { BufferType::INDEX, size, sizeof(uint32_t), id };
}

void Buffer::bind() const {
    GET_ID(id, _gl_buffer);

    auto typ = buffer_type_to_gl_enum(_typ);
    glBindBuffer(typ, id);
}

void Buffer::unbind(BufferType typ) {
    glBindBuffer(buffer_type_to_gl_enum(typ), 0);
}

Shader::Shader(uint32_t gl_shader)
    : _gl_shader(std::make_shared<uint32_t>(gl_shader))
{
}

Shader Shader::from_src(const char *vertex_src, const char *fragment_src) {
    uint32_t vertex, fragment, shader;
    gl_utils::compile_shader_module(GL_VERTEX_SHADER, vertex_src, &vertex);
    gl_utils::compile_shader_module(GL_FRAGMENT_SHADER, fragment_src, &fragment);
    gl_utils::link_shader_modules(vertex, fragment, &shader);
    return Shader {shader};
}

Shader::~Shader() {
    if (_gl_shader.has_value() && _gl_shader.value().use_count() == 1) {
        glDeleteProgram(*_gl_shader.value());
    }
}

void Shader::bind() {
    GET_ID(id, _gl_shader);
    glUseProgram(id);
}

void Shader::unbind() {
    glUseProgram(0);
}

int get_uniform_location(uint32_t id, const std::string &name) {
    auto location = glGetUniformLocation(id, name.c_str());
    if (location == -1) {
        WARN("Could not find uniform {}", name);
    }
    return location;
}

void Shader::set_int(const std::string &name, int32_t value) {
    GET_ID(id, _gl_shader);
    glUseProgram(id);
    int location = get_uniform_location(id, name);
    glUniform1i(location, value);
}

void Shader::set_float(const std::string &name, float value) {
    GET_ID(id, _gl_shader);
    glUseProgram(id);
    int location = get_uniform_location(id, name);
    glUniform1f(location, value);
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) {
    GET_ID(id, _gl_shader);
    glUseProgram(id);
    int location = get_uniform_location(id, name);
    glUniformMatrix4fv(location, 1, false, glm::value_ptr(mat));
}

FrameBuffer::~FrameBuffer() {
    if (_gl_frame_buffer.has_value() && _gl_frame_buffer.value().use_count() == 1) {
        glDeleteFramebuffers(1, &*_gl_frame_buffer.value());
    }
}

void FrameBuffer::bind() {
    GET_ID(id, _gl_frame_buffer);

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, _width, _height);
    //int dw = (int)_width / 2;
    //int dh = (int)_height / 2;
    //glViewport(-dw, -dh, (int)_width, (int)_height);
}

void FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer FrameBuffer::from_texture(const Texture &texture) {
    uint32_t fbo;
    glGenFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.native_texture(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ERROR("FrameBuffer::from_texture: framebuffer is not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto fb = FrameBuffer(texture.width(), texture.height(), fbo);
    fb._attachments.push_back(texture);

    return fb;
}

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, uint32_t gl_frame_buffer)
    : _gl_frame_buffer(std::make_shared<uint32_t>(gl_frame_buffer)), _width(width), _height(height)
{
}
