#version 300 es 
precision highp float;
in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 viewPos;
uniform vec4 baseColor;
uniform float realTime;

out vec4 finalColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPosition);

    vec3 lightDir = normalize(vec3(0.5f, 1.0f, 0.3f));
    float diff = max(dot(normal, lightDir), 0.0f);

    float rim = 1.0f - max(dot(normal, viewDir), 0.0f);
    rim = pow(rim, 3.0f); // 调整幂次改变边缘的宽度

    float pulse = (sin(realTime * 2.0f) * 0.5f + 0.5f) * 0.3f;
    vec3 rimColor = vec3(0.0f, 0.6f, 1.0f);

    vec3 finalRGB = baseColor.rgb * (diff + 0.2f) + (rimColor * (rim + pulse));
    finalColor = vec4(finalRGB, baseColor.a);
}