#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in float fragLifeRatio;

in vec3 vPosition;
flat in uint vID;
in float vRemainingLife;

out vec4 finalColor;

uniform sampler2D dataTex;
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

uniform sampler2D sceneDepth;
uniform sampler2D tex;

// 自定义

void main() {
    // 务必让fragTexCoord参与结果运算，否则内存访问会出错
    // float depth = texture(sceneDepth, fragTexCoord).r;

    float t = 1.0;
    // 反转y轴
    vec2 centeredCoord = fragTexCoord * 2 - 1;
    float dist = length(centeredCoord);
    if(dist > 1.0) {
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
    t = clamp(t / 10, 0, 1);
    vec4 texColor = texture(tex, fragTexCoord);
    finalColor = (texColor * (1 - t) + vec4(1, 0, 0, 1) * t);

}
