#version 300 es 
precision highp float;

in vec2 fragTexCoord;
in vec4 fragColor;
in float fragLifeRatio;
in vec3 vViewPos;
in vec3 vPosition;
flat in uint vID;
in float vRemainingLife;
in vec3 vDir;
in float vRadius;
in float vNear;
in float vFar;

out vec4 finalColor;

uniform mat4 matProj;
uniform highp sampler2D dataTex;
uniform int maxParticles;
vec4 GetPos(int id) {
    return texelFetch(dataTex, ivec2(0, id), 0);
}
vec3 GetVel(int id) {
    return texelFetch(dataTex, ivec2(1, id), 0).xyz;
}
vec4 GetAcc(int id) {
    return texelFetch(dataTex, ivec2(2, id), 0);
}
vec4 GetColor(int id) {
    return texelFetch(dataTex, ivec2(3, id), 0);
}
vec4 GetSize(int id) {
    return texelFetch(dataTex, ivec2(4, id), 0);
}
vec4 GetLife(int id) {
    return texelFetch(dataTex, ivec2(5, id), 0);
}

uniform highp sampler2D sceneDepth;
uniform highp sampler2D tex;

// 自定义

void main() {
    // 务必让fragTexCoord参与结果运算，否则内存访问会出错
    vec2 screenSize = vec2(textureSize(sceneDepth, 0));
    vec2 screenUV = gl_FragCoord.xy / screenSize;

    float depth = texture(sceneDepth, screenUV).r;

    vec2 normalCoords = fragTexCoord * 2.0f - 1.0f;
    float r2 = dot(normalCoords, normalCoords);
    if(r2 > 1.0f)
        discard;
    float z = sqrt(1.0f - r2);
    float thickness = 2.0f * vRadius * z;

    vec3 view = vPosition - vViewPos;
    float centerViewZ = dot(view, vDir);
    float pixelViewZ = centerViewZ - z * vRadius;
    vec4 clipPos = matProj * vec4(0.0f, 0.0f, -pixelViewZ, 1.0f);
    gl_FragDepth = (clipPos.z / clipPos.w) * 0.5f + 0.5f;
    finalColor = vec4(vec3(thickness), 1.0f);
}
