#version 300 es 
precision highp float;

in vec2 fragTexCoord;
out vec4 finalColor;

uniform highp sampler2D u_screen;

uniform float u_threshold;

void main() {
    vec4 color = texture(u_screen, fragTexCoord);
    float brightness = dot(color.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
    if(brightness > u_threshold) {
        finalColor = color;
    } else {
        finalColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}