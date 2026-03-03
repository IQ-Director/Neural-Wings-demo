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

uniform float u_cloudFrequency;
uniform float u_cloudSize;
uniform float u_cloudThickness;
uniform vec3 u_windDir;
uniform float u_windSpeed;
uniform float u_stretch;
uniform float u_distVar;
uniform float u_density;

uniform float MAX_STEPS;
uniform float MAX_DIST;
uniform float STEP_SIZE;

float hash(vec3 p) {
    p = fract(p * 0.56673f);
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
    for(int i = 0; i < 4; i++) {
        v += amp * noise(p);
        p *= 2.02f;
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
        vec3 windNormalized = normalize(u_windDir);

        vec3 pWarped = p - windNormalized * dot(p, windNormalized) * (1.0f - 1.0f / u_stretch);
        pWarped -= u_windDir * realTime * u_windSpeed;

        float coverage = noise(pWarped * u_cloudFrequency * 0.2f);
        float dynamicSize = u_cloudSize + (0.5f - coverage) * u_distVar;

        float baseDensity = fbm(pWarped * u_cloudFrequency);
        float density = smoothstep(dynamicSize, dynamicSize + u_cloudThickness, baseDensity);

        if(density > 0.0f) {
            float detail = fbm(pWarped * 0.3f + realTime * 0.1f);
            density *= detail;
        }

        density *= u_density;
        if(density > 0.001f) {
            vec3 localColor = mix(vec3(1.0f, 1.0f, 1.1f), vec3(0.8f, 0.9f, 1.0f), density * 2.0f);
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
