#version 330
layout(points) in;
layout(points, max_vertices = 30) out;

in int type0[];
in vec3 position0[];
in vec3 velocity0[];
in float age0[];

out int type1;
out vec3 position1;
out vec3 velocity1;
out float age1;

uniform float deltaTime;
uniform float time;
uniform sampler1D randomTexture;
uniform float launcherLifeTime;
uniform float shellLifeTime;
uniform float secondaryShellLifeTime; 

vec3 GetRandomDirection(float coord) {
	vec3 dir = texture(randomTexture, coord).xyz;
	dir -= vec3(0.5, 0.5, 0.5);
	return dir;
}

#define TYPE_LAUNCHER			0
#define TYPE_SHELL				1
#define TYPE_SECONDARY_SHELL	2

void main() {
	float age = age0[0] + deltaTime;
	if (type0[0] == TYPE_LAUNCHER) {
		if (age >= launcherLifeTime) {
			type1 = TYPE_SHELL;
			position1 = position0[0];
			vec3 dir = GetRandomDirection(time);
			dir.y = max(dir.y, 0.5);
			velocity1 = normalize(dir) / 20.f;
			age1 = 0;
			EmitVertex();
			EndPrimitive();
			age = 0;
		}

		type1 = TYPE_LAUNCHER;
		position1 = position0[0];
		velocity1 = velocity0[0];
		age1 = age;
		EmitVertex();
		EndPrimitive();
	}
	else {
		vec3 deltaPosition = deltaTime * velocity0[0];
		vec3 deltaVelocity = vec3(deltaTime) * vec3(0, -9.81, 0);

		if(type0[0] == TYPE_SHELL) {
			if (age < shellLifeTime) {
				type1 = TYPE_SHELL;
				position1 = position0[0] + deltaPosition;
				velocity1 = velocity0[0] + deltaVelocity;
				age1 = age;
				EmitVertex();
				EndPrimitive();
			}
			else {
				for(int i = 0; i < 10; ++i) {
					type1 = TYPE_SECONDARY_SHELL;
					position1 = position0[0];
					vec3 dir = GetRandomDirection(deltaTime + i / 1000.f);	
					velocity1 = normalize(dir) / 20.f;
					age1 = 0;
					EmitVertex();
					EndPrimitive();
				}
			}
		}
		else {
			if (age < secondaryShellLifeTime) {
				type1 = TYPE_SECONDARY_SHELL;
				position1 = position0[0] + deltaPosition;
				velocity1 = velocity0[0] + deltaVelocity;
				age1 = age;
				EmitVertex();
				EndPrimitive();
			}
		}
	}
}
