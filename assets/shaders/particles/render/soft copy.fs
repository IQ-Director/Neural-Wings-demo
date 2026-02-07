#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in float fragLifeRatio;

out vec4 finalColor;

// U引擎内置参数
uniform vec2 u_resolution;
uniform float u_near;
uniform float u_far;
uniform sampler2D u_sceneDepth;

// 自定义
uniform sampler2D u_diffuseMap;
uniform float u_softness;

// float LinerDepth(float depth) {
//     float z = depth * 2.0 - 1.0;
//     return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near)); // Linearize
// }

void main() {
    // vec4 texColor = texture(u_diffuseMap, fragTexCoord);
    // vec4 baseColor = texColor * fragColor;
    // vec2 screenUV = gl_FragCoord.xy / u_resolution;
    // float rawSceneDepth = texture(u_sceneDepth, screenUV).r;
    // float rawParticleDepth = gl_FragCoord.z;

    // float sceneDepth = LinerDepth(rawSceneDepth);
    // float particleDepth = LinerDepth(rawParticleDepth);

    // float depthDiff = sceneDepth - particleDepth;

    // float softFactor = smoothstep(0.0, u_softness, depthDiff);

    // finalColor = baseColor;
    // finalColor.a *= softFactor;
    // if(fragLifeRatio < 0.01) {
    //     discard;
    // }
    float depth = texture(u_sceneDepth, fragTexCoord).r;
    float use = (depth) * 0.000001;

    // 最后的输出强制受这些变量影响（即便效果看不出来）
    finalColor = vec4(1.0, 0.0, 0.0 + use, 1.0);

}
