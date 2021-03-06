#pragma once
#include <gl/glew.h>

#include "utilities.h"

namespace Globals {
	static const int kWindowWidth =  1024;
	static const int kWindowHeight = 768;

	static const float kFieldOfView = 3.141592f / 3.f;
	
	static const float kNearPlane = 1.f;
	static const float kFarPlane = 100.f;

	static const int kMaxParticleCount = 10000;
	static const int kMaxParticlePerFrame = int(0.016 * kMaxParticleCount);

	static const int kMaxBones = 100;

	static const GLfloat kQuadCoordinates[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	};

	enum {
		ColorTexture = GL_TEXTURE0,
		UVTexture,
		NormalTexture,
		MotionTexture,
		HDRTexture,
	};

	enum {
		ColorTextureIndex,
		UVTextureIndex,
		NormalTextureIndex,
		MotionTextureIndex,
		HDRTextureIndex,
	};
}

struct PointLight {
	glm::vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
	PADDING(12);
	glm::vec3 position;
	float constant;
	float linear;
	float exp;
};

struct DirectionalLight {
	glm::vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
	PADDING(12);
	glm::vec3 direction;
};
