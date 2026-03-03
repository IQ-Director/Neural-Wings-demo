#version 300 es 
precision highp float;

// 标准输入
in vec3 pPosition;
in vec3 pVelocity;
in vec3 pAcceleration;

in vec4 pColor;
in vec2 pSize;
in float pRotation;
in vec2 pLife; // (totalLife,remainingLife )
in uint pRandomID;
in uint pID;

// 标准输出
out vec4 outPosition; // (x,y,z,padding)
out vec4 outVelocity; // (x,y,z,padding)
out vec4 outAcceleration; // (x,y,z,padding)

out vec4 outColor;
out vec4 outSizeRotation; // (s1,s2,rotation,padding)
out vec4 outLifeRand; // (totalLife,remainingLife,randomID,ID )

// 引擎内置参数
uniform highp sampler2D dataTex;
uniform int maxParticles;
uniform float deltaTime;
uniform float gameTime;
uniform float realTime;
// SPH参数
const float H = 0.25f; // kernal radius;
const float H2 = H * H;
const float MASS = 1.0f; //粒子质量
const float Rho0 = 100.0f; //目标密度
const float GAS_CONST = 50.0f; // 压力数
const float VISC = 15.5f; //粘度
const float PI = 3.1415926535f;
const vec3 GRAVITY = vec3(0, -9.8f, 0);

// 边界
vec3 BOX_MIN = vec3(0.0f, -5.0f, 0.0f);
vec3 BOX_MAX = vec3(5.0f, 200.0f, 5.0f);

vec3 GetPos(int id) {
    return texelFetch(dataTex, ivec2(0, id), 0).xyz;
}
vec3 GetVel(int id) {
    return texelFetch(dataTex, ivec2(1, id), 0).xyz;
}
vec4 GetAcc(int id) {
    return texelFetch(dataTex, ivec2(2, id), 0);
}
vec4 GetColor(int id) {
    return texelFetch(dataTex, ivec2(3, id), 0);
}
vec4 GetSize(int id) {
    return texelFetch(dataTex, ivec2(4, id), 0);
}
vec4 GetLife(int id) {
    return texelFetch(dataTex, ivec2(5, id), 0);
}

// 自定义输入

void main() {
    float dt = deltaTime;
    float wavePeriod = 8.0f;
    float phase = mod(gameTime, wavePeriod);

    float pistonX = 0.0f;
    if(phase < 4.5f) {
        pistonX = (phase / 4.5f) * 25.0f;
    } else {
        pistonX = 25.0f - ((phase - 4.5f) / (8.0f - 4.5f)) * 25.0f;
    }
    BOX_MIN = vec3(-5.0f + 4.0f * sin(gameTime * 1.4f), -5.0f, -0.0f);
    BOX_MAX = vec3(5.0f + 2.0f * sin(gameTime * 2.0f), 200.0f, 5.0f);
    // for(int i = 0; i < maxParticles; i++) {
    //     if(uint(i) == pID)
    //         continue;
    //     // 去除死粒子
    //     if(GetLife(i).y <= 0)
    //         continue;

    //     float t = distance(pPosition, GetPos(i).xyz);
    //     if(t < 0.001)
    //         continue;
    // }

    // stage 1:密度计算
    float rho = 0.0f;
    int count = 0;
    for(int i = 0; i < maxParticles; i++) {
        // 去除死粒子
        if(GetLife(i).y <= 0.0f)
            continue;
        vec3 otherPos = GetPos(i);
        vec3 diff = pPosition - otherPos;
        float r2 = dot(diff, diff);
        if(r2 < H2) {
            count++;
              // Poly6 Kernel
            float h2_r2 = H2 - r2;
            rho += MASS * (315.0f / (64.0f * PI * pow(H, 9.0f))) * h2_r2 * h2_r2 * h2_r2;

        }
    }
    float size = clamp((float(count) - 10.0f) / 20.0f, -0.1f, 0.1f);
    float pS = clamp(pSize.x + pSize.x * size, 0.1f, 0.3f);

    vec2 newSize = vec2(pS);
    float pressure = (rho - Rho0) * GAS_CONST;
    vec3 fPress = vec3(0.0f);
    vec3 fVisc = vec3(0.0f);

    // stage 2:合力计算（压力+粘稠力）
    for(int i = 0; i < maxParticles; i++) {
        if(uint(i) == pID)
            continue;
        // 去除死粒子
        if(GetLife(i).y <= 0.0f)
            continue;
        vec3 otherPos = GetPos(i);
        vec3 diff = pPosition - otherPos;
        float r = length(diff);

        if(r < H && r > 0.001f) {
            vec3 dir = normalize(diff);
            // Spiky Kernal Gradient
            float gradW = -45.0f / (PI * pow(H, 6.0f)) * pow(H - r, 2.0f);
            fPress += dir * (-MASS * (pressure + GAS_CONST) / (2.0f * rho) * gradW);
            // Viscosity 
            vec3 otherVel = GetVel(i);
            float lapW = 45.0f / (PI * pow(H, 6.0f)) * (H - r);
            fVisc += (VISC * MASS * (otherVel - pVelocity) / (rho) * lapW);
        }
    }
    vec3 fBound = vec3(0);
    // if(BOX_MAX.z - pPosition.z < 0.15) {
    //     fBound = vec3(80.0, 0.0, 0.0);
    // }
    vec3 force = fPress + fVisc + fBound * rho + GRAVITY * rho;
    vec3 acc = force / rho + pAcceleration;
    vec3 newVelocity = pVelocity + acc * dt + vec3(float(pRandomID), 0, float(pRandomID)) * 0.000001f;
    newVelocity = newVelocity * 0.992f;
    vec3 newPosition = pPosition + newVelocity * dt;

    vec3 newAcceleration = vec3(0);

    float bounce = 1.0f;
    float aF = bounce;
    float pene = 0.0f;

    if(newPosition.x < BOX_MIN.x) {
        pene = BOX_MIN.x - newPosition.x;
        newPosition.x = BOX_MIN.x;
        newVelocity.x *= -bounce * pene;
        newAcceleration.x = aF;
    }
    if(newPosition.x > BOX_MAX.x) {
        pene = -BOX_MAX.x + newPosition.x;
        newPosition.x = BOX_MAX.x - 0.1f;
        newVelocity.x *= -bounce * pene * 1.0f;

        //newVelocity.y *= bounce * pene * 0;
        //newAcceleration.x = -aF * 100;
        //newAcceleration.y = aF * 100;
    }
    if(newPosition.y < BOX_MIN.y) {
        newPosition.y = BOX_MIN.y;
        newVelocity.y *= -bounce;
        newAcceleration.x = aF * 0.8f;
    }
    if(newPosition.y > BOX_MAX.y) {
        newPosition.y = BOX_MAX.y;
        newVelocity.y *= -bounce;
    }
    if(newPosition.z < BOX_MIN.z) {
        newPosition.z = BOX_MIN.z;
        newVelocity.z *= -bounce;
        newAcceleration.z = aF;
    }
    if(newPosition.z > BOX_MAX.z) {
        newPosition.z = BOX_MAX.z;
        newVelocity.z *= -bounce;
        newAcceleration.z = -aF;
    }

    outPosition = vec4(newPosition, 0);
    outVelocity = vec4(newVelocity, 0);
    outAcceleration = vec4(newAcceleration, 0);
    outColor = pColor;
    outSizeRotation = vec4(newSize, pRotation, 0);
    outLifeRand = vec4(pLife.x, pLife.y - dt, uintBitsToFloat(pRandomID), uintBitsToFloat(pID));
}