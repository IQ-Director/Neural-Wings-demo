#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in float fragLifeRatio;

out vec4 finalColor;

uniform sampler2D u_sceneDepth;
uniform sampler2D tex;
// 自定义

void main() {
    // 务必让fragTexCoord参与结果运算，否则内存访问会出错
    float depth = texture(u_sceneDepth, fragTexCoord).r;

    vec4 texColor = texture(tex, fragTexCoord);
    // 最后的输出强制受这些变量影响（即便效果看不出来）

    finalColor = texColor;

}
