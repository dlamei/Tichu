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
        // needed because some opengl versions don't support dynamic indexing of sampler arrays
        CASE_ID(0);
        CASE_ID(1);
        CASE_ID(2);
        CASE_ID(3);
        CASE_ID(4);
        CASE_ID(5);
        CASE_ID(6);
        CASE_ID(7);
        CASE_ID(8);
        CASE_ID(9);
        CASE_ID(10);
        CASE_ID(11);
        CASE_ID(12);
        CASE_ID(13);
        CASE_ID(14);
        CASE_ID(15);
        CASE_ID(16);
        CASE_ID(17);
        CASE_ID(18);
        CASE_ID(19);
        CASE_ID(20);
        CASE_ID(21);
        CASE_ID(22);
        CASE_ID(23);
        CASE_ID(24);
        CASE_ID(25);
        CASE_ID(26);
        CASE_ID(27);
        CASE_ID(28);
        CASE_ID(29);
        CASE_ID(30);
        CASE_ID(31);

        default:
            break;
    }

    frag_col *= vert_col;
}
)";
