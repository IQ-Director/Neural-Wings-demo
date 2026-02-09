#version 330

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
uniform sampler2D dataTex;
uniform int maxParticles;
uniform float deltaTime;

// SPH参数
const float H = 0.6; // kernal radius;
const float H2 = H * H;
const float MASS = 0.02; //粒子质量
const float Rho0 = 1000.0; //目标密度
const float GAS_CONST = 2000.0; // 压力数
const float VISC = 0.1; //粘度
const float PI = 3.1415926535;
const vec3 GRAVITY = vec3(0, -9.8, 0);

// 边界
const vec3 BOX_MIN = vec3(-3.0, 0.0, -3.0);
const vec3 BOX_MAX = vec3(3.0, 8.0, 3.0);

vec4 GetPos(int id) {
    return texelFetch(dataTex, ivec2(0, id), 0);
}
vec4 GetVel(int id) {
    return texelFetch(dataTex, ivec2(1, id), 0);
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
    vec3 dir = vec3(0);
    float dis2 = 1;
    vec3 a = vec3(0);
    for(int i = 0; i < maxParticles; i++) {
        if(uint(i) == pID)
            continue;
        // 去除死粒子
        if(GetLife(i).y <= 0)
            continue;

        float t = distance(pPosition, GetPos(i).xyz);
        if(t < 0.001)
            continue;
        dis2 = t * t;
        dir = -normalize(pPosition - GetPos(i).xyz);
        a += (dir * 1 / dis2);
    }
    vec3 newVelocity = pVelocity + (a) * dt;

    vec3 newPosition = pPosition + newVelocity * dt;

    vec2 newSize = pSize;

    outPosition = vec4(newPosition, 0);
    outVelocity = vec4(newVelocity, 0);
    outAcceleration = vec4(pAcceleration, 0);
    outColor = pColor;
    outSizeRotation = vec4(newSize, pRotation, 0);
    outLifeRand = vec4(pLife.x, pLife.y - dt, uintBitsToFloat(pRandomID), uintBitsToFloat(pID));
}