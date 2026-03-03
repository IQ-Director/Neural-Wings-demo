#version 300 es 
precision highp float;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform highp sampler2D rawScreen;
void main() {

    finalColor = texture(rawScreen, fragTexCoord);
}