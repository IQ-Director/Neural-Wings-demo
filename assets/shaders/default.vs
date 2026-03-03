#version 300 es 
precision highp float;
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

uniform mat4 u_mvp;
uniform mat4 transform;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out float v_linearDepth;

void main() {
    fragPosition = vec3(transform * vec4(vertexPosition, 1.0f));
    fragTexCoord = vertexTexCoord;
    fragNormal = normalize(vec3(transform * vec4(vertexNormal, 0.0f)));
    gl_Position = u_mvp * vec4(vertexPosition, 1.0f);
    v_linearDepth = gl_Position.w;
}