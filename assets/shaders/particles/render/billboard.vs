#version 330

// raylib提供
in vec3 vertexPosition; // loc 0
in vec2 vertexTexCoord;

// 实例数据（来自TFB Buffer）

in vec3 pPosition; // loc 2
in vec3 pVelocity;
in vec3 pAcceleration;

in vec4 pColor;
in vec2 pSize;
in float pRotation;
in vec2 pLife; // (totalLife,remainingLife )
in uint pRandomID;
in uint pID;

// uniform
uniform vec3 viewPos;
uniform float gameTime; // 游戏时间
uniform float realTime;
uniform vec4 baseColor;

uniform mat4 vp; // View*Projection
uniform mat4 model;//  局部系或世界系
uniform vec3 cameraRight; // 相机右向量（用于Billboard）
uniform vec3 cameraUp; // 相机上向量（用于Billboard）

out vec2 fragTexCoord;
out vec4 fragColor;
out float fragLifeRatio; // 归一化寿命

out vec3 vPosition;
flat out uint vID;
out float vRemainingLife;

void main() {
    float lifeRatio = clamp(pLife.y / pLife.x, 0.0, 1.0);
    if(pLife.y <= 0.00001) {
        gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }
    // 计算局部系还是世界系。
    vec3 centerWorld = (model * vec4(pPosition, 1.0)).xyz;

    vec3 billboardPos = centerWorld +
        cameraRight * pSize.x * vertexPosition.x +
        cameraUp * pSize.y * vertexPosition.y;

    float c = cos(pRotation);
    float s = sin(pRotation);
    vec3 rotateOffset = cameraRight * (vertexPosition.x * c - vertexPosition.y * s) * pSize.x +
        cameraUp * (vertexPosition.x * s + vertexPosition.y * c) * pSize.y;
    vec3 finalPos = billboardPos + rotateOffset;

    gl_Position = vp * vec4(finalPos, 1.0);
    fragTexCoord = vec2(vertexTexCoord.x, -vertexTexCoord.y);
    fragColor = pColor;
    fragLifeRatio = lifeRatio;

    vPosition = pPosition;
    vID = pID;
    vRemainingLife = pLife.y;
}