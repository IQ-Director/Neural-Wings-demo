#version 300 es
precision highp float;

in vec2 fragTexCoord;
out vec4 finalColor;

uniform highp sampler2D u_scene;
uniform highp sampler2D u_scene_depth;

uniform sampler2D u_charAtlas;     // ASCII 字符集贴图 (建议 1x16 或 1x10)
uniform vec2 screenResolution;
uniform float realTime;
uniform float cameraNear;
uniform float cameraFar;

uniform float u_fontSize;   // 字符大小
uniform float u_edgeThreshold;
uniform float u_ditherSoftness;
uniform float u_lineWidth;
uniform float u_charCount;
// 亮度计算
float getLuminance(vec3 color) {
    return dot(color, vec3(0.299f, 0.587f, 0.114f));
}

float LinearizeDepth(vec2 uv) {
    float d = texture(u_scene_depth, uv).r;
    float z = d * 2.0f - 1.0f;
    return (2.0f * cameraNear * cameraFar) / (cameraFar + cameraNear - z * (cameraFar - cameraNear));
}
float drawEdgeLine(vec2 localUV, vec2 grad) {
    vec2 n = normalize(grad);

    float dist = abs(dot(localUV - vec2(0.5f), n));

    return smoothstep(u_lineWidth, u_lineWidth - 0.05f, dist);
}

void main() {
    vec2 cells = screenResolution / u_fontSize;
    vec2 cellUV = floor(fragTexCoord * cells) / cells;
    vec2 localUV = fract(fragTexCoord * cells);

    // 1. 获取中心和邻域采样用于描边检测
    vec4 centerCol = texture(u_scene, cellUV);
    float centerDepth = LinearizeDepth(cellUV);

    // 简单的边缘检测 (利用深度差异)
    vec2 texel = 1.0f / cells;
    float dL = LinearizeDepth(cellUV + vec2(-texel.x, 0.0f));
    float dR = LinearizeDepth(cellUV + vec2(texel.x, 0.0f));
    float dU = LinearizeDepth(cellUV + vec2(0.0f, texel.y));
    float dD = LinearizeDepth(cellUV + vec2(0.0f, -texel.y));
    vec2 grad = vec2(dR - dL, dU - dD);
    float edgeStrength = length(grad) * 10.0f; // 根据需要调整缩放系数

    float gray = getLuminance(centerCol.rgb);
    float scaledGray = gray * (u_charCount - 1.0f);
    float indexA = floor(scaledGray);
    float indexB = ceil(scaledGray);
    float t = fract(scaledGray); // 混合比例
    vec2 charUVA = vec2((localUV.x + indexA) / u_charCount, localUV.y);
    float valA = texture(u_charAtlas, charUVA).r;
    vec2 charUVB = vec2((localUV.x + indexB) / u_charCount, localUV.y);
    float valB = texture(u_charAtlas, charUVB).r;
    float charMask = mix(valA, valB, t * u_ditherSoftness);
    float finalMask;
    vec3 charColor = centerCol.rgb;

    if(edgeStrength > u_edgeThreshold && centerDepth < 150.0f) {
        float lineMask = drawEdgeLine(localUV, grad);
        float edgeAlpha = clamp(edgeStrength * 0.5f, 0.0f, 1.0f);
        finalMask = mix(charMask, lineMask, edgeAlpha);

        charColor *= 1.5f;
    } else {
        finalMask = charMask;
    }
    float bgMask = 0.01f;
    vec3 finalRGB = charColor * max(finalMask, bgMask);

    finalColor = vec4(finalRGB, centerCol.a);
}