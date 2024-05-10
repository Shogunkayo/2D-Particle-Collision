#shader vertex
#version 330 core

layout(location = 0) in vec4 a_Position;

out vec4 u_Update;

uniform mat4 u_MVP;
uniform vec4 u_Updates[4];

void main() {
    u_Update = u_Updates[gl_InstanceID];
    gl_Position = u_MVP * (a_Position + vec4(u_Update.xy, 0.0, 0.0));
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 u_Update;

uniform vec4 u_Color;

void main() {
    if (distance(u_Update.xy, gl_FragCoord.xy) < u_Update.z)
        color = u_Color;
    else
        discard;
}
