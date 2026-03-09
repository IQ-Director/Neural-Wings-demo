#version 300 es
precision highp float;

in vec3 vNormal;
in vec3 vWorldPos;

uniform vec3 viewPos;
uniform vec4 baseColor;
uniform float realTime;

out vec4 finalColor;

void main() {
    vec3 n = normalize(vNormal);
    vec3 v = normalize(viewPos - vWorldPos);

    // 1. 菲涅尔计算：视线垂直于表面法线时（边缘）最亮
    // 使用 abs(dot) 是为了让球体的背面（透过正面看到的）也能产生边缘光
    float fresnel = 1.0f - abs(dot(n, v));
    fresnel = pow(fresnel, 3.0f); // 调整幂次改变边缘锐度
    // 2. 动态脉冲：模拟能量流动的起伏
    float pulse = sin(realTime * 2.0f) * 0.1f + 0.9f;

    // 3. 简单的网格线（可选）
    float grid = sin(vWorldPos.y * 5.0f - realTime * 3.0f) * 0.5f + 0.5f;
    grid = step(0.97f, grid) * 0.2f;

    // 4. 合成
    vec3 color = baseColor.rgb * 1.5f; // 稍微提亮
    float alpha = (fresnel + grid) * baseColor.a * pulse;

    finalColor = vec4(color, alpha);
}