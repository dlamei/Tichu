const char *VERTEX_SRC = R"(
#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_col;
layout (location = 3) in float in_tex_id;

out vec2 vert_uv;
out vec4 vert_col;
flat out int vert_tex_id;

uniform mat4 u_camera;


void main() {
    vert_uv = in_uv;
    vert_col = in_col;
    vert_tex_id = int(in_tex_id);
    gl_Position = u_camera * vec4(in_pos, 1.0);
}
)";

const char *FRAGMENT_SRC = R"(
#version 330 core
in vec2 vert_uv;
in vec4 vert_col;
flat in int vert_tex_id;

uniform sampler2D u_texture_slots[32];

out vec4 frag_col;

#define CASE_ID(id) \
    case id: \
        frag_col = texture(u_texture_slots[id], vert_uv); \
        break;


void main() {
    frag_col = vec4(1.f, 0.f, 1.f, 1.f);

    switch (vert_tex_id) {
        // needed because some opengl compilers don't support dynamic indexing of sampler arrays
        // also macros don't work on macos?
    case 0:
        frag_col = texture(u_texture_slots[0], vert_uv);
        break;
    case 1:
        frag_col = texture(u_texture_slots[1], vert_uv);
        break;
    case 2:
        frag_col = texture(u_texture_slots[2], vert_uv);
        break;
    case 3:
        frag_col = texture(u_texture_slots[3], vert_uv);
        break;
    case 4:
        frag_col = texture(u_texture_slots[4], vert_uv);
        break;
    case 5:
        frag_col = texture(u_texture_slots[5], vert_uv);
        break;
    case 6:
        frag_col = texture(u_texture_slots[6], vert_uv);
        break;
    case 7:
        frag_col = texture(u_texture_slots[7], vert_uv);
        break;
    case 8:
        frag_col = texture(u_texture_slots[8], vert_uv);
        break;
    case 9:
        frag_col = texture(u_texture_slots[9], vert_uv);
        break;
    case 10:
        frag_col = texture(u_texture_slots[10], vert_uv);
        break;
    case 11:
        frag_col = texture(u_texture_slots[11], vert_uv);
        break;
    case 12:
        frag_col = texture(u_texture_slots[12], vert_uv);
        break;
    case 13:
        frag_col = texture(u_texture_slots[13], vert_uv);
        break;
    case 14:
        frag_col = texture(u_texture_slots[14], vert_uv);
        break;
    case 15:
        frag_col = texture(u_texture_slots[15], vert_uv);
        break;
    default:
        break;
    }

    frag_col *= vert_col;
}
)";
