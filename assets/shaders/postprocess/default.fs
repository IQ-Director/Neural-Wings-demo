#version 300 es 
precision mediump float;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D rawScreen;
void main() {

    finalColor = texture(rawScreen, fragTexCoord);
}