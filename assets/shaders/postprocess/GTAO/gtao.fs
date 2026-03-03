#version 300 es 
precision highp float;
in vec2 fragTexCoord;
out vec4 finalColor;

// uniform highp sampler2D u_rawScreen;
uniform highp sampler2D u_rawScreen_depth;

uniform vec3 cameraPosition;
uniform vec3 cameraDir;
uniform vec3 cameraUp;
uniform vec3 cameraRight;
uniform float cameraFov;
uniform vec2 screenResolution;
uniform float cameraFar;
uniform float cameraNear;
uniform float gameTime;

uniform float u_radius;
uniform float u_slices;
uniform float u_steps;
uniform float u_intensity;
uniform float u_near;
uniform float u_far;

const float PI = 3.1415926f;

vec3 getViewPos(vec2 uv) {
    float d = texture(u_rawScreen_depth, uv).r;
    float z = d * 2.0f - 1.0f;
    float linearZ = (2.0f * cameraNear * cameraFar) / (cameraFar + cameraNear - z * (cameraFar - cameraNear));
    float aspect = screenResolution.x / screenResolution.y;
    float tanHalfFov = tan(radians(cameraFov) * 0.5f);
    vec2 ndc = uv * 2.0f - 1.0f;

    vec3 viewRay = vec3(ndc.x * aspect * tanHalfFov, ndc.y * tanHalfFov, -1.0f);
    return viewRay * linearZ;
}

vec3 getViewNormal(vec3 p, vec2 uv) {
    vec2 invRes = 1.0f / screenResolution;
    float d = texture(u_rawScreen_depth, uv).r;
    float dL = texture(u_rawScreen_depth, uv + vec2(-invRes.x, 0.0f)).r;
    float dR = texture(u_rawScreen_depth, uv + vec2(invRes.x, 0.0f)).r;
    float dU = texture(u_rawScreen_depth, uv + vec2(0.0f, invRes.y)).r;
    float dD = texture(u_rawScreen_depth, uv + vec2(0.0f, -invRes.y)).r;

    vec3 vL = p - getViewPos(uv + vec2(-invRes.x, 0.0f));
    vec3 vR = getViewPos(uv + vec2(invRes.x, 0.0f)) - p;
    vec3 vD = p - getViewPos(uv + vec2(0.0f, -invRes.y));
    vec3 vU = getViewPos(uv + vec2(0.0f, invRes.y)) - p;

    vec3 dx = (abs(dL - d) < abs(dR - d)) ? vL : vR;
    vec3 dy = (abs(dD - d) < abs(dU - d)) ? vD : vU;

    return normalize(cross(dx, dy));
}

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898f, 78.233f))) * 43758.5453f);
}
float IntegrateArc(float h1, float h2, float n) {
    return 0.25f * ((cos(n) + 2.0f * h1 * sin(n) - cos(2.0f * h1 - n)) +
        (cos(n) + 2.0f * h2 * sin(n) - cos(2.0f * h2 - n)));
}

void main() {
    float depth = texture(u_rawScreen_depth, fragTexCoord).r;
    if(depth >= 0.999999f) { // 跳过背景
        finalColor = vec4(1.0f);
        return;
    }
    vec3 P = getViewPos(fragTexCoord);
    vec3 N = getViewNormal(P, fragTexCoord);
    vec3 V = normalize(-P);

    float focalLength = screenResolution.y / (2.0f * tan(radians(cameraFov) * 0.5f));
    float screenRadius = (u_radius * focalLength) / abs(P.z);
    float ao = 0.0f;
    float noise = hash(fragTexCoord + fract(gameTime));

    for(int i = 0; i < int(u_slices); i++) {
        float angle = (float(i) + noise) * (PI / float(u_slices));
        vec3 sliceDir = vec3(cos(angle), sin(angle), 0.0f);

        vec3 planeNormal = normalize(cross(sliceDir, V));
        vec3 tangent = cross(V, planeNormal);
        vec3 projN = N - planeNormal * dot(N, planeNormal);
        float projNLen = max(length(projN), 0.0001f);
        vec3 projN_norm = projN / projNLen;

        float cos_n = clamp(dot(projN_norm, V), -1.0f, 1.0f);
        float n = sign(dot(projN_norm, tangent)) * acos(cos_n);

        float h1 = -PI / 2.0f;
        float h2 = PI / 2.0f;

        for(int j = 1; j <= int(u_steps); j++) {
            float stepOffset = (float(j) - 0.5f + noise) / u_steps;
            stepOffset *= stepOffset;

            vec2 uvOffset = sliceDir.xy * stepOffset * screenRadius / screenResolution;
            vec2 uvs[2] = vec2[](fragTexCoord + uvOffset, fragTexCoord - uvOffset);

              // 采样点 1
            vec3 S1 = getViewPos(uvs[0]);
            vec3 D1 = S1 - P;
            float dLen1 = length(D1);
            if(dLen1 > 0.001f && dLen1 < u_radius) {
                float cos1 = clamp(dot(normalize(D1), V), -1.0f, 1.0f);
                float angle1 = acos(cos1);
                float falloff = clamp(dLen1 / u_radius, 0.0f, 1.0f);
                angle1 = mix(angle1, PI / 2.0f, falloff * falloff);
                h2 = min(h2, angle1);
            }

            // 采样点 2
            vec3 S2 = getViewPos(uvs[1]);
            vec3 D2 = S2 - P;
            float dLen2 = length(D2);
            if(dLen2 > 0.001f && dLen2 < u_radius) {
                float cos2 = clamp(dot(normalize(D2), V), -1.0f, 1.0f);
                float angle2 = -acos(cos2);
                float falloff = clamp(dLen2 / u_radius, 0.0f, 1.0f);
                angle2 = mix(angle2, -PI / 2.0f, falloff * falloff);
                h1 = max(h1, angle2);
            }
        }
        h1 = clamp(h1, n - PI / 2.0f, n);
        h2 = clamp(h2, n, n + PI / 2.0f);
        ao += projNLen * IntegrateArc(h1, h2, n);
    }
    ao = clamp(ao / u_slices, 0.0f, 1.0f);
    float distanceFade = smoothstep(u_near, u_far, abs(P.z));
    ao = mix(ao, 1.0f, distanceFade);

    ao = pow(ao, u_intensity);
    finalColor = vec4(vec3(ao), 1.0f);
}