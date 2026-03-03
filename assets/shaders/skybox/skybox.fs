#version 300 es 
precision highp float;

in vec3 fragPosition;
out vec4 finalColor;

uniform highp samplerCube skyboxMap;
uniform vec4 tintColor;

void main() {
    vec3 color = texture(skyboxMap, fragPosition).rgb;

    color = color / (color + vec3(1.0f));
    color = pow(color, vec3(1.0f / 2.2f));

    finalColor = vec4(color, 1.0f) * tintColor;
}