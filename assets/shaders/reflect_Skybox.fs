#version 300 es 
precision highp float;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in float v_linearDepth;

uniform samplerCube skyboxMap;
uniform mat4 u_mvp;
uniform mat4 tranform;
uniform vec3 viewPos;
uniform float realTime;
uniform float gameTime;
uniform vec4 baseColor;

uniform highp sampler2D u_diffuseMap;
uniform int u_diffuseMap_frameCount;
uniform float u_diffuseMap_animSpeed;

out vec4 finalColor;
void main() {
    vec3 N = normalize(fragNormal);
    vec3 I = normalize(fragPosition - viewPos);
    vec3 R = reflect(I, N);
    vec3 reflectionColor = texture(skyboxMap, R).rgb;
    reflectionColor = reflectionColor / (reflectionColor + vec3(1.0f));
    reflectionColor = pow(reflectionColor, vec3(1.0f / 2.2f));

    float fresnel = 0.54f + 0.94f * pow(1.0f - max(dot(N, -I), 0.0f), 5.0f);

    float currentFrame = floor(mod(gameTime * u_diffuseMap_animSpeed, float(u_diffuseMap_frameCount)));
    vec2 animatedUV = fragTexCoord;
    animatedUV.y /= float(u_diffuseMap_frameCount);
    animatedUV.y += (currentFrame / float(u_diffuseMap_frameCount));

    vec4 texColor = texture(u_diffuseMap, animatedUV);

    vec3 ambient = vec3(0.3f);
    vec3 lightDir = normalize(vec3(0.5f, 1.0f, 0.3f));
    float diff = max(dot(fragNormal, lightDir), 0.0f);
    vec3 diffuse = vec3(diff);
    vec3 t = (texColor.rgb * baseColor.rgb) * (ambient + diffuse);

    vec3 result = mix(t, reflectionColor, fresnel);
    finalColor = vec4(result, texColor.a * baseColor.a);
}