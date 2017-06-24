#include "mesh.h"
#include "example.h"

class Example_SkeletalAnimationWithAssimp : public Example {
public:
	Example_SkeletalAnimationWithAssimp() {
		position_ = glm::vec3(0, 0, 6.f);

		camera_->Reset(glm::vec3(0, 3.f, -1.f), glm::vec3(0, 0, 1.f));

		shader_->Load(ShaderTypeVertex, "shaders/skinning.vert");
		shader_->Load(ShaderTypeFragment, "shaders/skinning.frag");
		shader_->Link();

		shader_->SetUniform("colorMap", Globals::ColorTextureIndex);
		//shader_->SetBlock("DirectionalLight", &dl_);
		//shader_->SetUniform("matSpecularIntensity", 0.f);
		//shader_->SetUniform("matSpecularPower", 0);

		skinnedMesh_.Load("models/boblampclean.md5mesh");

		boneLocation_ = glGetUniformLocation(shader_->GetProgram(), "bones");
	}

	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);

		shader_->Use();

		time_ += deltaTime;

		std::vector<glm::mat4> transforms;
		skinnedMesh_.GetBoneTransform(time_, transforms);

		glUniformMatrix4fv(boneLocation_, transforms.size(), GL_FALSE, (GLfloat*)&transforms[0]);

		//shader_->SetUniform("cameraPosition", &camera_->GetPosition());

		glm::quat q(glm::vec3(270.f, 180.f, 0));
		glm::mat4 M(1);

		glm::mat4 scaleT = glm::scale(M, glm::vec3(0.1f));
		glm::mat4 rotT = glm::toMat4(q);
		glm::mat4 translateT = glm::translate(M, position_);
		M = translateT * rotT * scaleT;

		shader_->SetUniform("M", &M);

		glm::mat4 VP = camera_->GetProjMatrix() * camera_->GetViewMatrix();
		glm::mat4 MVP = VP * M;
		shader_->SetUniform("MVP", &MVP);

		skinnedMesh_.Render();
	}

private:
	float time_;
	glm::vec3 position_;
	SkinnedMesh skinnedMesh_;

	PointLight pl_;
	DirectionalLight dl_;

	GLuint boneLocation_;
};
