#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

out vec4 v_Position;

void main() {
    gl_Position = position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Position;

uniform vec4 u_Color;
uniform float u_Size;
uniform vec4 u_Center;

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(720, 720) * 2.0 - 1.0;

    if (distance(uv, u_Center.xy) < 0.25) {
        color = u_Color;
    }
    else {
        discard;
    }
}
