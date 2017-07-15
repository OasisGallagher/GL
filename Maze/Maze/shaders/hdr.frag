#version 330

in VertOut {
	vec2 texCoord;
}fragIn;

out vec4 fragColor;

uniform bool HDR;
uniform sampler2D tex;
uniform sampler2D blur;
uniform float exposure = 1.f;

const float gamma = 2.2f;

void main() {
	vec3 color = texture(tex, fragIn.texCoord).rgb;
	
	if (HDR) {
		color += texture(blur, fragIn.texCoord).rgb;
		color = vec3(1.0) - exp(-color * exposure);
	}

	color = pow(color, vec3(1 / gamma));
	fragColor = vec4(color, 1);
}
