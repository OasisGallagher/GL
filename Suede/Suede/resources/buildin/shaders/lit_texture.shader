Properties {
}

SubShader {
	Pass {
		Blend SrcAlpha OneMinusSrcAlpha;

		GLSLPROGRAM

		varying vec2 texCoord;
		varying vec3 worldPos;
		varying vec3 normal;

		#stage vertex
		in vec3 c_position;
		in vec2 c_texCoord;
		in vec3 c_normal;

		#include "buildin/shaders/include/light_vertex.inc"

		uniform mat4 c_localToClipSpaceMatrix;
		uniform mat4 c_localToWorldSpaceMatrix;

		void main() {
			texCoord = c_texCoord;

			normal = (c_localToWorldSpaceMatrix * vec4(c_normal, 0)).xyz;
			worldPos = (c_localToWorldSpaceMatrix * vec4(c_position, 1)).xyz;
	
			calculateShadowCoord();

			gl_Position = c_localToClipSpaceMatrix * vec4(c_position, 1);
		}

		#stage fragment
		out vec4 fragColor;

		uniform vec4 c_mainColor;
		uniform sampler2D c_mainTexture;

		#include "buildin/shaders/include/light_fragment.inc"

		void main() {
			vec4 albedo = texture(c_mainTexture, texCoord) * c_mainColor;
			float visibility = calculateShadowVisibility();
			fragColor = albedo * vec4(calculateDirectionalLight(worldPos, normalize(normal), visibility), 1);
			fragColor.a = 0.7;
		}

		ENDGLSL
	}

	Pass {
		Blend SrcAlpha OneMinusSrcAlpha;

		GLSLPROGRAM

		varying vec2 texCoord;
		varying vec3 worldPos;
		varying vec3 normal;

		#stage vertex
		in vec3 c_position;
		in vec2 c_texCoord;
		in vec3 c_normal;

		#include "buildin/shaders/include/light_vertex.inc"

		uniform mat4 c_localToClipSpaceMatrix;
		uniform mat4 c_localToWorldSpaceMatrix;

		void main() {
			texCoord = c_texCoord;

			normal = (c_localToWorldSpaceMatrix * vec4(c_normal, 0)).xyz;
			worldPos = (c_localToWorldSpaceMatrix * vec4(c_position, 1)).xyz;
	
			calculateShadowCoord();

			gl_Position = c_localToClipSpaceMatrix * vec4(c_position, 1);
		}

		#stage fragment
		out vec4 fragColor;

		uniform vec4 c_mainColor;
		uniform sampler2D c_mainTexture;

		#include "buildin/shaders/include/light_fragment.inc"

		void main() {
			vec4 albedo = texture(c_mainTexture, texCoord) * c_mainColor;
			float visibility = calculateShadowVisibility();
			//fragColor = albedo * vec4(calculateDirectionalLight(worldPos, normalize(normal), visibility), 1);
			fragColor = vec4(normalize(normal), 0.3);
		}

		ENDGLSL
	}
}
