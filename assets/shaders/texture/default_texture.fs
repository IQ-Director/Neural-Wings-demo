#version 330
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform sampler2D u_diffuseMap;
uniform vec4 baseColor;

out vec4 finalColor;

void main() {
    vec4 texColor = texture(u_diffuseMap, fragTexCoord);

    vec3 ambient = vec3(0.3);

    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(fragNormal, lightDir), 0.0);
    vec3 diffuse = vec3(diff);
    vec3 t = (texColor.rgb * baseColor.rgb) * (ambient + diffuse);

    finalColor = vec4(t, texColor.a * baseColor.a);
}