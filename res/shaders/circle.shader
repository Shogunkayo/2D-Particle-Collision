#shader vertex
#version 430 core

layout(location = 0) in vec4 a_Position;

out vec4 u_Update;
out vec4 u_Color;

uniform mat4 u_MVP;
uniform vec4 u_Updates[10];
uniform vec4 u_Colors[10];

void main() {
    u_Color = u_Colors[gl_InstanceID];
    u_Update = u_Updates[gl_InstanceID];
    gl_Position = u_MVP * (a_Position + vec4(u_Update.xy, 0.0, 0.0));
};

#shader fragment
#version 430 core

layout(location = 0) out vec4 color;

in vec4 u_Update;
in vec4 u_Color;

void main() {

    float d = u_Update.z - distance(u_Update.xy, gl_FragCoord.xy);
    color = u_Color;
    color.a = exp(d);
}
