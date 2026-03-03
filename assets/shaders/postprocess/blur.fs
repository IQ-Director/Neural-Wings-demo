#version 300 es 
precision highp float;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform highp sampler2D u_screen;
uniform vec2 u_direction;
uniform float u_radius;

const float weights[5] = float[](0.14446f, 0.23336f, 0.13553f, 0.05135f, 0.00994f);
const float offsets[5] = float[](0.00000f, 1.41176f, 3.29411f, 5.17647f, 7.05882f);

void main() {
    vec2 tex_size = vec2(textureSize(u_screen, 0));
    vec2 tex_offset = 1.0f / tex_size;

    vec3 result = texture(u_screen, fragTexCoord).rgb * weights[0];

    for(int i = 1; i < 5; i++) {
        vec2 offset = u_direction * tex_offset * offsets[i] * u_radius;
        result += texture(u_screen, fragTexCoord + offset).rgb * weights[i];
        result += texture(u_screen, fragTexCoord - offset).rgb * weights[i];
    }
    finalColor = vec4(result, 1.0f);
}