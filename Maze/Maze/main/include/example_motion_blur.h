#include "mesh.h"
#include "example.h"

class MotionBlurFramebuffer {
private:
	enum {
		ColorBuffer,
		MotionBuffer,
		__testBuffer,
		DepthBuffer,
		BufferCount
	};

public:
	MotionBlurFramebuffer() {
		glGenFramebuffers(1, &fbo_);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);

		glGenTextures(COUNT_OF(buffers_), buffers_);

		glBindTexture(GL_TEXTURE_2D, buffers_[ColorBuffer]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Globals::kWindowWidth, Globals::kWindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffers_[ColorBuffer], 0);

		glBindTexture(GL_TEXTURE_2D, buffers_[MotionBuffer]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Globals::kWindowWidth, Globals::kWindowWidth, 0, GL_RG, GL_FLOAT, nullptr);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, buffers_[MotionBuffer], 0);

		glBindTexture(GL_TEXTURE_2D, buffers_[__testBuffer]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Globals::kWindowWidth, Globals::kWindowWidth, 0, GL_RG, GL_FLOAT, nullptr);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, buffers_[__testBuffer], 0);

		glBindTexture(GL_TEXTURE_2D, buffers_[DepthBuffer]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Globals::kWindowWidth, Globals::kWindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffers_[DepthBuffer], 0);

		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(COUNT_OF(drawBuffers), drawBuffers);

		Assert(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "invalid framebuffer");

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	~MotionBlurFramebuffer() {
		glDeleteFramebuffers(1, &fbo_);
		glDeleteBuffers(COUNT_OF(buffers_), buffers_);
	}

public:
	void Write() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
	}

	void __testBlit() {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
		glReadBuffer(GL_COLOR_ATTACHMENT2);
	}

	GLuint __testGetColorTexture() {
		return buffers_[ColorBuffer];
	}

	void Read() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glActiveTexture(Globals::ColorTexture);
		glBindTexture(GL_TEXTURE_2D, buffers_[ColorBuffer]);

		glActiveTexture(Globals::MotionTexture);
		glBindTexture(GL_TEXTURE_2D, buffers_[MotionBuffer]);
	}

private:
	GLuint fbo_;
	GLuint buffers_[BufferCount];
};

class Example_MotionBlur : public Example {
public:
	Example_MotionBlur() {
		shader_->Load(ShaderTypeVertex, "shaders/motion_blur_render.vert");
		shader_->Load(ShaderTypeFragment, "shaders/motion_blur_render.frag");
		shader_->Link();

		motionBlurShader_.Load(ShaderTypeVertex, "shaders/motion_blur.vert");
		motionBlurShader_.Load(ShaderTypeFragment, "shaders/motion_blur.frag");
		motionBlurShader_.Link();
		motionBlurShader_.SetUniform("colorMap", Globals::ColorTextureIndex);
		motionBlurShader_.SetUniform("motionBlurMap", Globals::MotionTextureIndex);
		
		mesh_.Load("models/boblampclean.md5mesh");
		mesh_.GetBoneTransform(0.f, prevTransforms);

		quad_.Load("models/quad_r.obj");

		camera_->Reset(glm::vec3(0, -2.5f, 1.1f), glm::vec3(0, 0, 0.5f));

		prevBoneLocation_ = glGetUniformLocation(shader_->GetProgram(), "prevBones");
		boneLocation_ = glGetUniformLocation(shader_->GetProgram(), "bones");

		radian_ = 0.f;
		pos_ = glm::vec3(0, 0, 0);
	}

	~Example_MotionBlur() {
	}

public:
	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);
		Input* input = app.GetInput();

		float moveSpeed = 8.f;
		if (input->IsKeyDown(KeyCodeLeft)) {
			pos_ -= glm::vec3(1, 0, 0) * deltaTime * moveSpeed;
		}

		if (input->IsKeyDown(KeyCodeRight)) {
			pos_ += glm::vec3(1, 0, 0) * deltaTime * moveSpeed;
		}

		time_ += deltaTime;

		radian_ = fmod(radian_ + deltaTime, glm::pi<float>());

		RenderPass();

// 		buffer_.__testBlit();
// 		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
// #define BLIT_FRAMEBUFFER_RECT 0, 0, Globals::kWindowWidth, Globals::kWindowHeight
// 		glBlitFramebuffer(BLIT_FRAMEBUFFER_RECT, BLIT_FRAMEBUFFER_RECT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		MotionBlurPass();
	}

private:
	void RenderPass() {
		buffer_.Write();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader_->Use();

		std::vector<glm::mat4> transforms;
		mesh_.GetBoneTransform(time_, transforms);

		glUniformMatrix4fv(prevBoneLocation_, prevTransforms.size(), GL_FALSE, (GLfloat*)&prevTransforms[0]);
		glUniformMatrix4fv(boneLocation_, transforms.size(), GL_FALSE, (GLfloat*)&transforms[0]);

		prevTransforms = transforms;

		glm::mat4 M(1);

		glm::mat4 scaleT = glm::scale(M, glm::vec3(0.02f));
		glm::mat4 translateT = glm::translate(M, pos_);
		M = translateT * scaleT;

		//shader_->SetUniform("M", &M);

		glm::mat4 VP = camera_->GetProjMatrix() * camera_->GetViewMatrix();
		glm::mat4 MVP = VP * M;
		shader_->SetUniform("MVP", &MVP);

		mesh_.Render();
	}

	void MotionBlurPass() {
		buffer_.Read();

		motionBlurShader_.Use();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		quad_.Render();
	}

private:
	Mesh quad_;
	float radian_;
	float time_;
	glm::vec3 pos_;
	SkinnedMesh mesh_;
	GLuint prevBoneLocation_;
	Shader motionBlurShader_;
	GLuint boneLocation_;
	MotionBlurFramebuffer buffer_;
	std::vector<glm::mat4> prevTransforms;
};
