#version 300 es 
precision highp float;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform highp sampler2D u_screen;
uniform highp sampler2D u_screen_depth;
uniform mat4 invVP;
uniform vec3 cameraPosition;
uniform float cameraFar;
uniform float cameraNear;
uniform float realTime;

uniform float MAX_STEPS;
uniform float MAX_DIST;
uniform float STEP_SIZE;

float hash(vec3 p) {
    p = fract(p * 0.1031f);
    p += dot(p, p.yzx + 33.33f);
    return fract((p.x + p.y) * p.z);
}

float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0f - 2.0f * f);
    return mix(mix(mix(hash(i + vec3(0, 0, 0)), hash(i + vec3(1, 0, 0)), f.x), mix(hash(i + vec3(0, 1, 0)), hash(i + vec3(1, 1, 0)), f.x), f.y), mix(mix(hash(i + vec3(0, 0, 1)), hash(i + vec3(1, 0, 1)), f.x), mix(hash(i + vec3(0, 1, 1)), hash(i + vec3(1, 1, 1)), f.x), f.y), f.z);
}

float fbm(vec3 p) {
    float v = 0.0f;
    float amp = 0.5f;
    for(int i = 0; i < 3; i++) {
        v += amp * noise(p);
        p *= 2.0f;
        amp *= 0.5f;
    }
    return v;
}
float LinearizeDepth(vec2 uv) {
    float d = texture(u_screen_depth, uv).r;
    float z = d * 2.0f - 1.0f;
    return (2.0f * cameraNear * cameraFar) / (cameraFar + cameraNear - z * (cameraFar - cameraNear));
}

void main() {
    vec4 sceneColor = texture(u_screen, fragTexCoord);

    vec4 farPos = invVP * vec4(fragTexCoord * 2.0f - 1.0f, 1.0f, 1.0f);
    vec3 rayDir = normalize(farPos.xyz / farPos.w - cameraPosition);

    float sceneDistance = LinearizeDepth(fragTexCoord);
    float marchDist = min(sceneDistance, MAX_DIST);

    float transmittance = 1.0f;
    vec3 scatteredLight = vec3(0.0f);

    float offset = hash(vec3(fragTexCoord * 1000.0f, realTime)) * STEP_SIZE;
    for(int i = 0; i < int(MAX_STEPS); i++) {
        float d = float(i) * STEP_SIZE + offset;
        if(d > marchDist)
            break;
        vec3 p = cameraPosition + rayDir * d;

        float density = fbm(p * 0.1f + vec3(realTime * 0.02f));
        density = smoothstep(0.4f, 0.8f, density) * 0.15f;

        if(density > 0.01f) {
            vec3 localColor = mix(vec3(1, 1, 1), vec3(0.4f, 0.2f, 0.5f), density);
            float transparency = exp(-density * STEP_SIZE);
            scatteredLight += localColor * (transmittance * (1.0f - transparency));
            transmittance *= transparency;
        }
        if(transmittance < 0.01f)
            break;
    }
    vec3 finalRGB = sceneColor.rgb * transmittance + scatteredLight;
    finalColor = vec4(finalRGB, sceneColor.a);
}