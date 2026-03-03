#version 300 es 
precision highp float;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform highp sampler2D u_rawScreen;
uniform highp sampler2D u_rawScreen_depth;

uniform vec3 cameraPosition;
uniform vec3 cameraDir;
uniform vec3 cameraUp;
uniform vec3 cameraRight;
uniform float cameraFov;
uniform vec2 screenResolution;
uniform float gameTime;

float getDepth(vec2 uv) {

    float cameraFar = 1000.0f;
    float cameraNear = 0.1f;

    float d = texture(u_rawScreen_depth, uv).r;
    // float z = d * 2.0f - 1.0f;
    // return (2.0f * cameraNear * cameraFar) / (cameraFar + cameraNear - z * (cameraFar - cameraNear));
    return d;
}

void main() {
    float depth = getDepth(fragTexCoord);
    vec4 color = texture(u_rawScreen, fragTexCoord);
    finalColor = vec4(vec3(depth * 100.0f), 1.0f);

    // finalColor = color;
}