#version 330
in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D rawScreen;
void main() {

    finalColor = texture(rawScreen, fragTexCoord);
}