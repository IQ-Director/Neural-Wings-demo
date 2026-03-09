#version 300 es
precision highp float;

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D u_scene;
uniform vec2 screenResolution;

// 控制参数
uniform float u_threshold;      // 基础亮度阈值
uniform float u_satThreshold;   // 饱和度启发阈值
uniform float u_intensity;      // 泛光总强度
uniform float u_vibrancy;       // 颜色鲜艳度增强
uniform float u_blurSize;

// 【核心】饱和度提取函数
vec3 GetVibrantBright(vec3 color) {
    float maxC = max(color.r, max(color.g, color.b));
    float minC = min(color.r, min(color.g, color.b));
    float luminance = dot(color, vec3(0.2126f, 0.7152f, 0.0722f));

    // 计算饱和度 (防止除以零)
    float saturation = (maxC - minC) / (maxC + 0.001f);

    // 判定逻辑：如果是高亮度 OR 高饱和度
    // 这意味着鲜艳的颜色（即便不亮）也会产生泛光
    float weight = smoothstep(u_threshold, u_threshold + 0.2f, luminance);
    weight += smoothstep(u_satThreshold, u_satThreshold + 0.2f, saturation);
    weight = clamp(weight, 0.0f, 1.0f);

    // 增强提取出的色彩，使其在模糊后依然保持鲜艳
    return color * weight * u_vibrancy;
}

void main() {
    vec2 texelSize = u_blurSize / screenResolution;
    vec4 baseColor = texture(u_scene, fragTexCoord);

    // 1. 初始化 Bloom 颜色
    vec3 bloom = GetVibrantBright(baseColor.rgb);

    // 2. 8-Tap 偏移采样 (利用不同的偏移步长模拟高斯分布)
    vec2 offsets[8] = vec2[](vec2(1.0f, 0.0f), vec2(-1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(0.0f, -1.0f), vec2(0.7f, 0.7f), vec2(-0.7f, 0.7f), vec2(0.7f, -0.7f), vec2(-0.7f, -0.7f));

    for(int i = 0; i < 8; i++) {
        vec3 s1 = texture(u_scene, fragTexCoord + offsets[i] * texelSize).rgb;
        vec3 s2 = texture(u_scene, fragTexCoord + offsets[i] * texelSize * 2.0f).rgb;

        bloom += GetVibrantBright(s1) * 0.5f;
        bloom += GetVibrantBright(s2) * 0.25f;
    }

    bloom /= 4.0f;
    bloom *= u_intensity;

    // 3. 最终合成：Base + Bloom
    // 使用 Screen 混合或简单的 Additive
    // 技巧：让 Bloom 稍微影响一下 Base 的颜色，增加整体融合感
    vec3 finalRGB = baseColor.rgb + bloom;

    // 4. 可选：后期的饱和度微调 (Color Grading)
    // 让最终画面色彩更浓郁
    float finalLum = dot(finalRGB, vec3(0.2126f, 0.7152f, 0.0722f));
    finalRGB = mix(vec3(finalLum), finalRGB, 1.2f);

    finalColor = vec4(finalRGB, baseColor.a);
}