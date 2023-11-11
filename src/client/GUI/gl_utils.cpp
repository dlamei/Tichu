#include "gl_utils.h"
#include <stb_image.h>
#include <cstddef>
#include <algorithm>
#include "../../src/common/logging.h"

#define INTERNAL_FORMAT GL_RGBA8
#define EXTERNAL_FORMAT GL_RGBA

#define GL_VERTEX_BUFFER GL_ARRAY_BUFFER
#define GL_INDEX_BUFFER GL_ELEMENT_ARRAY_BUFFER

void Vertex::bind_layout() {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
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

Texture::~Texture() {
    if (_gl_texture.has_value() && _gl_texture.value().unique()) {
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
    if (!_gl_texture.has_value()) {
        WARN("Texture::bind was not initialized!");
        return;
    }

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
            return GL_ARRAY_BUFFER;
        default:
            ASSERT(false, "found unknown BufferType");
    }
}

Buffer::Buffer(BufferType typ, uint32_t size, uint32_t stride, uint32_t gl_buffer)
    : _typ(typ), _size(size), _stride(stride), _gl_buffer(std::make_shared<uint32_t>(gl_buffer))
{

}

Buffer::~Buffer() {
    if (_gl_buffer.has_value() && _gl_buffer.value().unique()) {
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

Buffer Buffer::index(void *data, uint32_t size, uint32_t stride) {
    uint32_t id;
    gl_utils::create_buffer(GL_INDEX_BUFFER, &id, data, size);
    return Buffer { BufferType::INDEX, size, stride, id };
}

void Buffer::bind() const {
    if (!_gl_buffer.has_value()) {
        WARN("Buffer::bind was not initialized!");
        return;
    }

    glBindBuffer(buffer_type_to_gl_enum(_typ), *_gl_buffer.value());
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
    if (_gl_shader.has_value() && _gl_shader.value().unique()) {
        glDeleteProgram(*_gl_shader.value());
    }
}

void Shader::bind() {
    if (!_gl_shader.has_value()) {
        WARN("Shader::bind was not initialized!");
        return;
    }

    glUseProgram(*_gl_shader.value());
}

void Shader::unbind() {
    glUseProgram(0);
}

FrameBuffer::~FrameBuffer() {
    if (_gl_frame_buffer.has_value() && _gl_frame_buffer.value().unique()) {
        glDeleteFramebuffers(1, &*_gl_frame_buffer.value());
    }
}

void FrameBuffer::bind() {
    if (!_gl_frame_buffer.has_value()) {
        WARN("FrameBuffer::bind was not initialized!");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, *_gl_frame_buffer.value());
    int dw = (int)_width / 2;
    int dh = (int)_height / 2;
    glViewport(-dw, -dh, (int)_width, (int)_height);
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

void draw_impl(const Buffer &buffer) {
    glDrawArrays(GL_TRIANGLES, 0, buffer.size() / buffer.stride());
}

