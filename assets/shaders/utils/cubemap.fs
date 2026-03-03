#version 300 es 
precision highp float;

in vec3 fragPosition;
out vec4 finalColor;

uniform highp sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591f, 0.3183f);

vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5f;
    uv.y = 1.0f - uv.y;
    return uv;
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(fragPosition));
    // 采样 HDR 颜色
    vec3 color = texture(equirectangularMap, uv).rgb;
    finalColor = vec4(color, 1.0f);
}