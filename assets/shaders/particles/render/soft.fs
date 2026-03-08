#version 300 es 
precision highp float;

in vec2 fragTexCoord;
in vec4 fragColor;
in float fragLifeRatio;

in vec3 vPosition;
flat in uint vID;
in float vRemainingLife;

out vec4 finalColor;

uniform int tex_frameCount;
uniform float tex_animSpeed;
uniform highp sampler2D dataTex;
uniform int maxParticles;
uniform float gameTime;
uniform float realTime;
uniform vec4 baseColor;

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

    t = length(GetVel(int(vID)));
    t = clamp(t / 10.0f, 0.0f, 1.0f);
    float currentFrame = floor(mod(gameTime * tex_animSpeed, float(tex_frameCount)));

    vec2 animatedUV = fragTexCoord;
    animatedUV.y /= float(tex_frameCount);

    animatedUV.y += (currentFrame / float(tex_frameCount));

    vec4 texColor = texture(tex, animatedUV) * baseColor;

    finalColor = (texColor * (1.0f - t) + vec4(1.0f, 0.0f, 0.0f, 1.0f) * t);

}
