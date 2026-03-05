#version 300 es 
precision highp float;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform highp sampler2D u_diffuseMap;
uniform int u_diffuseMap_frameCount;
uniform float u_diffuseMap_animSpeed;
uniform vec4 baseColor;
uniform vec4 totalBaseColor;
uniform float gameTime;
uniform float realTime;

out vec4 finalColor;

void main() {
    float currentFrame = floor(mod(gameTime * u_diffuseMap_animSpeed, float(u_diffuseMap_frameCount)));
    vec2 animatedUV = fragTexCoord;
    animatedUV.y /= float(u_diffuseMap_frameCount);

    animatedUV.y += (currentFrame / float(u_diffuseMap_frameCount));

    vec4 texColor = texture(u_diffuseMap, animatedUV);

    vec3 ambient = vec3(0.3f);

    vec3 lightDir = normalize(vec3(0.5f, 1.0f, 0.3f));
    float diff = max(dot(fragNormal, lightDir), 0.0f);
    vec3 diffuse = vec3(diff);
    vec3 t = (texColor.rgb * baseColor.rgb * totalBaseColor.rgb) * (ambient + diffuse);

    finalColor = vec4(t, texColor.a * baseColor.a * totalBaseColor.a);
}