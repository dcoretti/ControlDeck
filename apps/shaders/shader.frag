#version 430 core
in vec2 texCoordUv;
out vec3 fragColor;

uniform sampler2D texSampler;
void main() {
	fragColor = texture(texSampler, texCoordUv).rgb;
    //vec3(1.0,0.0,0.0);
}