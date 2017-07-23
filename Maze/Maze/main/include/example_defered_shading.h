
#pragma once
#include <cmath>
#include "mesh.h"
#include "example.h"

class GBuffer {
public:
	enum TextureUnit {
		PositionTextureUnit = 0,
		DiffuseTextureUnit = 1,
		NormalTextureUnit = 2,
		UnitCount,
	};

public:
	GBuffer() {
		fbo_ = 0;
		depthTexture_ = 0;
		finalTexture_ = 0;
		memset(textures_, 0, sizeof(textures_));
	}

	~GBuffer() {
		glDeleteFramebuffers(1, &fbo_);
		glDeleteTextures(COUNT_OF(textures_), textures_);
		glDeleteTextures(1, &depthTexture_);
		glDeleteTextures(1, &finalTexture_);
	}

	bool Init() {
		glGenFramebuffers(1, &fbo_);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

		Assert(glIsFramebuffer(fbo_), "invalid framebuffer");

		glGenTextures(COUNT_OF(textures_), textures_);
		glGenTextures(1, &depthTexture_);
		glGenTextures(1, &finalTexture_);

		for (unsigned i = 0; i < COUNT_OF(textures_); ++i) {
			glBindTexture(GL_TEXTURE_2D, textures_[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Globals::kWindowWidth, Globals::kWindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures_[i], 0);
		}

		glBindTexture(GL_TEXTURE_2D, depthTexture_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, Globals::kWindowWidth, Globals::kWindowHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0);

		glBindTexture(GL_TEXTURE_2D, finalTexture_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Globals::kWindowWidth, Globals::kWindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, finalTexture_, 0);

		Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "invalid framebuffer status");
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		return true;
	}

	void StartFrame() {
		// At the start of each frame we need to clear the final texture which is attached to attachment point number 4.
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
		glDrawBuffer(GL_COLOR_ATTACHMENT4);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void BindForGeometryPass() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
		GLenum drawBuffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
		};

		glDrawBuffers(COUNT_OF(drawBuffers), drawBuffers);
	}

	void BindForStencilPass() {
		// Disable the draw buffers.
		// As mentioned earlier, in the stencil test we are not writing to the color buffer,
		// only the stencil buffer. Indeed, even our FS is empty. 
		// However, in that case the default output color from the FS is black. 
		// In order to avoid garbaging the final buffer with a black image of the bounding sphere
		// we disable the draw buffers here.
		glDrawBuffer(GL_NONE);
	}

	void BindForLightPass() {
		glDrawBuffer(GL_COLOR_ATTACHMENT4);
		for (unsigned i = 0; i < COUNT_OF(textures_); ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures_[PositionTextureUnit + i]);
		}
	}

	void BindForFinalPass() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
		glReadBuffer(GL_COLOR_ATTACHMENT4);
	}

private:
	GLuint fbo_;
	GLuint textures_[UnitCount];
	GLuint finalTexture_;
	GLuint depthTexture_;
};

class Example_DeferedShading : public Example {
public:
	Example_DeferedShading() {
		gbuffer_.Init();

		shader_->Load(ShaderTypeVertex, "shaders/gbuffer.vert");
		shader_->Load(ShaderTypeFragment, "shaders/gbuffer.frag");
		shader_->Link();

		glm::vec2 screenSize(Globals::kWindowWidth, Globals::kWindowHeight);

		ps_.Load(ShaderTypeVertex, "shaders/deferred_light.vert");
		ps_.Load(ShaderTypeFragment, "shaders/deferred_point_light.frag");
		ps_.Link();
		ps_.SetUniform("positionMap", GBuffer::PositionTextureUnit);
		ps_.SetUniform("colorMap", GBuffer::DiffuseTextureUnit);
		ps_.SetUniform("normalMap", GBuffer::NormalTextureUnit);
		ps_.SetUniform("screenSize", &screenSize);
		
		ds_.Load(ShaderTypeVertex, "shaders/deferred_light.vert");
		ds_.Load(ShaderTypeFragment, "shaders/deferred_dir_light.frag");
		ds_.Link();
		ds_.SetUniform("positionMap", GBuffer::PositionTextureUnit);
		ds_.SetUniform("colorMap", GBuffer::DiffuseTextureUnit);
		ds_.SetUniform("normalMap", GBuffer::NormalTextureUnit);
		ds_.SetUniform("screenSize", &screenSize);
		
		null_.Load(ShaderTypeVertex, "shaders/null.vert");
		null_.Load(ShaderTypeFragment, "shaders/null.frag");
		null_.Link();

		box_.Load("models/box.obj");
		quad_.Load("models/quad.obj");
		mesh_.Load("models/phoenix_ugv.md2");
		sphere_.Load("models/sphere.obj");

		boxPositions_[0] = glm::vec3(0.0f, 0.0f, 5.0f);
		boxPositions_[1] = glm::vec3(6.0f, 1.0f, 10.0f);
		boxPositions_[2] = glm::vec3(-5.0f, -1.0f, 12.0f);
		boxPositions_[3] = glm::vec3(4.0f, 4.0f, 15.0f);
		boxPositions_[4] = glm::vec3(-4.0f, 2.0f, 20.0f);

		dl_.color = glm::vec3(0, 1, 1);
		dl_.ambientIntensity = 0.1f;
		dl_.diffuseIntensity = 0.5f;
		dl_.direction = glm::vec3(1, 0, 0);

		pl_[0].diffuseIntensity = 0.2f;
		pl_[0].color = glm::vec3(0, 1, 0);
		pl_[0].position = glm::vec3(0.0f, 1.5f, 5.0f);
		pl_[0].constant = 0.0f;
		pl_[0].linear = 0.0f;
		pl_[0].exp = 0.3f;

		pl_[1].diffuseIntensity = 0.2f;
		pl_[1].color = glm::vec3(1, 0, 0);
		pl_[1].position = glm::vec3(2.0f, 0.0f, 5.0f);
		pl_[1].constant = 0.0f;
		pl_[1].linear = 0.0f;
		pl_[1].exp = 0.3f;

		pl_[2].diffuseIntensity = 0.2f;
		pl_[2].color = glm::vec3(0, 0, 1);
		pl_[2].position = glm::vec3(0.0f, 0.0f, 3.0f);
		pl_[2].constant = 0.0f;
		pl_[2].linear = 0.0f;
		pl_[2].exp = 0.3f;

		camera_->Reset(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
	}

	virtual void GetEnvRequirement(AppEnv& env) {
		Example::GetEnvRequirement(env);
		env.backgroundColor = glm::vec4(0, 0, 0, 1);
		env.cullFace = false;
	}

	virtual void Update(float deltaTime) {
		Example::Update(deltaTime);
		UpdateRotation(deltaTime);

		gbuffer_.StartFrame();

		GeometryPass();

		//gbuffer_.BindForLightPass();
		// We need stencil to be enabled in the stencil pass to get the stencil buffer
		// updated and we also need it in the light pass because we render the light
		// only if the stencil passes.
		glEnable(GL_STENCIL_TEST);
		
		for (unsigned i = 0; i < COUNT_OF(pl_); ++i) {
			StencilPass(i);
			PointLightsPass(i);
		}
		
		glDisable(GL_STENCIL_TEST);
		
		DirectionalLightsPass();

		FinalPass();
	}

private:
	void UpdateRotation(float deltaTime) {
		rot_ += deltaTime * 0.25f;

		if (rot_ > glm::pi<float>() * 2) {
			rot_ -= glm::pi<float>() * 2;
		}
	}

	void GeometryPass() {
		shader_->Bind();

		gbuffer_.BindForGeometryPass();

		// Only the geometry pass updates the depth buffer.
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_BLEND);

		for (int i = 0; i < COUNT_OF(boxPositions_); ++i) {
			glm::mat4 M = glm::mat4(1);
			M = glm::translate(M, glm::vec3(boxPositions_[i])) * glm::rotate(M, rot_, glm::vec3(0, 1, 0));

			glm::mat4 MVP = camera_->GetProjMatrix() * camera_->GetViewMatrix() * M;
			shader_->SetUniform("M", &M);
			shader_->SetUniform("MVP", &MVP);

			shader_->SetUniform("sampler", Globals::ColorTextureIndex);

			box_.Render();
		}

		// When we get here the depth buffer is already populated and the stencil
		// pass depends on it, but it does not write to it.
		glDepthMask(GL_FALSE);
		//glDisable(GL_DEPTH_TEST);
	}

	// The stencil buffer is connected with the Stencil Test which is a per-fragment operation
	// we are seeing here for the first time. In a similar manner to the depth test, the stencil 
	// test can be used to discard pixels prior to pixel shader execution. It works by comparing
	// the value at the current pixel location in the stencil buffer with a reference value. 
	void StencilPass(unsigned index) {
		null_.Bind();

		gbuffer_.BindForStencilPass();
		// We enable depth test here because the stencil operation depends on it.
		glEnable(GL_DEPTH_TEST);

		// We disable culling because we want to process both the front and back faces of each polygon.
		glDisable(GL_CULL_FACE);

		glClear(GL_STENCIL_BUFFER_BIT);

		// We need the stencil test to be enabled but we want it
		// to succeed always. Only the depth test matters.
		glStencilFunc(GL_ALWAYS, 0, 0);

		// Configure the stencil operation for the back facing polygons to increment the value in the
		// stencil buffer when the depth test fails but to keep it unchanged when either depth test or
		// stencil test succeed.
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		// Only the stencil buffer is updated.
		float scale = CalculatePointLightBoundingSphere(pl_[index]);

		glm::mat4 MVP(1);
		MVP = glm::translate(MVP, pl_[index].position) * glm::scale(MVP, glm::vec3(scale));
		MVP = camera_->GetProjMatrix() * camera_->GetViewMatrix() * MVP;
		null_.SetUniform("MVP", &MVP);

		// Since the geometry was already rendered and the depth buffer is populated we can check what
		// happens to the depth test when the vector goes through the front and back pixels of the sphere
		// and update the stencil buffer accordingly...
		// http://ogldev.atspace.co.uk/www/tutorial37/tutorial37.html
		sphere_.Render();

		// When we are done the stencil buffer contains positive values only in the pixels of objects
		// inside the light volume.
	}

	void PointLightsPass(int index) {
		gbuffer_.BindForLightPass();

		ps_.Bind();
		ps_.SetUniform("cameraPosition", &camera_->GetPosition());

		// It sets up the stencil test to pass when the stencil value is not equal to zero.
		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
		
		//  We don't need it and on some GPUs we may get some performance by disabling it.
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		// We enable culling of the front face polygons. The reason why we do that is because the camera
		// may be inside the light volume and if we do back face culling as we normally do we will not see 
		// the light until we exit its volume.
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		ps_.SetBlock("PointLight", &pl_[index]);

		float scale = CalculatePointLightBoundingSphere(pl_[index]);

		glm::mat4 MVP(1);
		MVP = glm::translate(MVP, pl_[index].position) * glm::scale(MVP, glm::vec3(scale));
		MVP = camera_->GetProjMatrix() * camera_->GetViewMatrix() * MVP;
		ps_.SetUniform("MVP", &MVP);

		sphere_.Render();
		
		glCullFace(GL_BACK);
		glDisable(GL_BLEND);

		// ??????????? 如果注释下面这行, 方向光的效果不对.
		glDisable(GL_CULL_FACE);
	}

	void FinalPass() {
		gbuffer_.BindForFinalPass();
		// In the final pass we blit from the color buffer inside the G Buffer into the screen.
		// This is a good place to discuss why we added an intermediate color buffer in the G Buffer 
		// instead of rendering directly to the screen. The thing is that our G Buffer combines as a 
		// target the buffers for the attributes with the depth/stencil buffer. When we run the point 
		// light pass we setup the stencil stuff and we need to use the values from the depth buffer. 
		// Here we have a problem - if we render into the default FBO we won't have access to the depth
		// buffer from the G Buffer. But the G Buffer must have its own depth buffer because when we render 
		// into its FBO we don't have access to the depth buffer from the default FBO. 
		// Therefore, the solution is to add to the G Buffer FBO a color buffer to render into and in the 
		// final pass blit it to the default FBO color buffer. 
		glBlitFramebuffer(0, 0, Globals::kWindowWidth, Globals::kWindowHeight, 0, 0, Globals::kWindowWidth, Globals::kWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}

	float CalculatePointLightBoundingSphere(const PointLight& point) {
		float maxChannel = glm::max(glm::max(point.color.x, point.color.y), point.color.z);
		float delta = point.linear * point.linear - 4 * point.exp * (point.exp - 256 * maxChannel * point.diffuseIntensity);
		return (-point.linear + sqrtf(delta)) / (2 * point.exp);
	}

	void DirectionalLightsPass() {
		gbuffer_.BindForLightPass();

		ds_.Bind();
		ds_.SetUniform("cameraPosition", &camera_->GetPosition());
		
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		ds_.SetBlock("DirectionalLight", &dl_);

		glm::mat4 MVP(1);
		ds_.SetUniform("MVP", &MVP);
		
		quad_.Render();

		glDisable(GL_BLEND);
	}

private:
	float rot_;

	Shader ps_;
	Shader ds_;
	Shader null_;

	PointLight pl_[3];
	DirectionalLight dl_;

	glm::vec3 boxPositions_[5];

	Mesh box_;
	Mesh quad_;
	Mesh mesh_;
	Mesh sphere_;
	GBuffer gbuffer_;
};
