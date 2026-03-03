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
    // float depth = texture(sceneDepth, fragTexCoord).r;

    float t = 1.0f;
    // 反转y轴
    vec2 centeredCoord = fragTexCoord * 2.0f - 1.0f;
    float dist = length(centeredCoord);
    if(dist > 1.0f) {
        discard;
    }
    // for(int i = 0; i < maxParticles; i++) {
    //     if(uint(i) == vID)
    //         continue;
    //     if(vRemainingLife <= 0)
    //         continue;
    //     float a = distance(vPosition, GetPos(i).xyz);

    //     t = min(a, t);
    // }
    t = length(GetVel(int(vID)));
    float s = GetSize(int(vID)).r;
    s = clamp((s - 0.04f) * 1.5f, 0.0f, 1.0f);
    t = clamp(t / 10.0f, 0.0f, 1.0f);
    vec4 texColor = texture(tex, fragTexCoord);

    vec3 view = vPosition - vViewPos;
    float centerViewZ = dot(view, vDir);
    float pixelViewZ = centerViewZ - 1.0f * vRadius;
    vec4 clipPos = matProj * vec4(0.0f, 0.0f, -pixelViewZ, 1.0f);
    gl_FragDepth = (clipPos.z / clipPos.w) * 0.5f + 0.5f;
    vec4 velColor = texColor * (1.0f - t) + vec4(1.0f) * t;
    finalColor = velColor;

}
