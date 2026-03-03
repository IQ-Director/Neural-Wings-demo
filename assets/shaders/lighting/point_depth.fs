#version 300 es 
precision highp float;
in vec3 vWorldPos;

uniform vec3 lightPos;
uniform float farPlane;

out vec4 fragColor;

void main() {
    float lightDistance = length(vWorldPos - lightPos);
    lightDistance = lightDistance / farPlane;
    gl_FragDepth = lightDistance;

    fragColor = vec4(lightDistance, lightDistance, lightDistance, 1.0f);
}