#version 300 es 
precision highp float;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform highp sampler2D u_normal;
uniform highp sampler2D u_simpleFluid;
uniform highp sampler2D u_thickness;
uniform highp sampler2D u_fluidDepth;
uniform highp sampler2D u_sceneTex;
uniform samplerCube skyboxMap;

uniform vec3 cameraPosition;
uniform vec3 cameraDir;
uniform vec3 cameraUp;
uniform vec3 cameraRight;
uniform float cameraFov;
uniform vec2 screenResolution;

uniform vec3 lightDir;

vec3 getWorldPos(vec2 uv) {
    float d = texture(u_fluidDepth, uv).r;
    if(d <= 0.0f || d >= 1000.0f)
        return vec3(0.0f);
    float radFov = radians(cameraFov);
    float tanHalfFov = tan(radFov * 0.5f);

    vec2 ndc = uv * 2.0f - 1.0f;
    float aspect = float(screenResolution.x) / float(screenResolution.y);
    vec3 rayDir = cameraDir +
        (cameraRight * ndc.x * aspect * tanHalfFov) +
        (cameraUp * ndc.y * tanHalfFov);
    vec3 worldRay = normalize(rayDir);
    return cameraPosition + worldRay * d;

}

void main() {
    vec3 worldPos = getWorldPos(fragTexCoord);
    // vec4 centerColor = texture(rawScreen, fragTexCoord);
    vec4 screenColor = texture(u_sceneTex, fragTexCoord);
    float thickness = texture(u_thickness, fragTexCoord).r;
    float depth = texture(u_fluidDepth, fragTexCoord).r;
    vec4 fluidColor = texture(u_simpleFluid, fragTexCoord);
    if(depth <= 0.01f || depth >= 1000.0f) {
        finalColor = screenColor;
        return;
    }

    vec3 viewDir = normalize(worldPos - cameraPosition);
    vec3 normal = texture(u_normal, fragTexCoord).rgb * 2.0f - 1.0f;

//折射  
    float refractionStrength = 0.05f;
    vec2 refractionUV = fragTexCoord + normal.xy * refractionStrength * min(1.0f, thickness);
    vec3 refractionColor = texture(u_sceneTex, refractionUV).rgb;

// 吸收
    vec3 liquidColor = vec3(0.1f, 0.5f, 0.8f);
    float absorption = exp(-thickness * 0.2f);
    vec3 transmittedColor = mix(liquidColor, refractionColor, absorption);

//高光
    vec3 r = viewDir - normal * 2.0f * dot(normal, viewDir);

    float spec = pow(max(dot(-lightDir, r), 0.0f), 128.0f);

    vec3 specularColor = vec3(1.0f) * spec;

// 反射

    vec3 envColor = texture(skyboxMap, r).rgb;
    envColor = envColor / (envColor + vec3(1.0f));
    envColor = pow(envColor, vec3(1.0f / 2.2f));
    float fresnel = 0.02f + 0.98f * pow(1.0f - max(dot(normal, -viewDir), 0.0f), 5.0f);

    float brightness = dot(fluidColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));

    vec3 finalRGB = mix(transmittedColor, envColor, fresnel) + specularColor + fluidColor.rgb * 0.0f;

    float edgeAlpha = smoothstep(0.00f, 0.1f, thickness);

    finalColor = vec4(finalRGB, edgeAlpha);
}