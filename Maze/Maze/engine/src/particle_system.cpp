#include "shader.h"
#include "texture.h"
#include "utilities.h"
#include "particle_system.h"

ParticleSystem::ParticleSystem() {
	time_ = 0;

	currentVB_ = 0;
	currentTFB_ = 1;

	isFirst_ = true;
}

ParticleSystem::~ParticleSystem() {
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(COUNT_OF(particleBuffer_), particleBuffer_);
	glDeleteTransformFeedbacks(COUNT_OF(transformFeedback_), transformFeedback_);

	delete updateShader_;
	delete billboardShader_;
	
	delete texture_;
	delete randomTexture_;
}

bool ParticleSystem::Init(const glm::vec3& position) {
	Particle particles[kMaxParticles];
	memset(particles, 0, sizeof(particles));

	// The remaining particles will be created at render time.
	particles[0].type = 0;
	particles[0].position = position;
	particles[0].velocity = glm::vec3(0, 0.0001f, 0);
	particles[0].lifeTime = 0;

	// OpenGL enforces a general limitation that the same resource cannot be bound 
	// for both input and output in the same draw call.This means that if we want to
	// update the particles in a vertex buffer we actually need two transform feedback 
	// buffers and toggle between them.

	// We are going to use two transform feedback buffers and toggle between them 
	// (drawing into one while using the other as input and vice verse).
	glGenTransformFeedbacks(COUNT_OF(transformFeedback_), transformFeedback_);

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(COUNT_OF(particleBuffer_), particleBuffer_);

	for (int i = 0; i < COUNT_OF(transformFeedback_); ++i) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback_[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer_[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);

		// This makes this buffer a transform feedback buffer and places it as index zero.
		// We can have the primitives redirected into more than one buffer by binding several buffers at different indices. 
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer_[i]);

		// Now we have two transform feedback objects with corresponding buffer objects that
		// can serve both as vertex buffers as well as transform feedback buffers.
	}

	texture_ = new Texture;
	texture_->Load("textures/fireworks_red.jpg");

	randomTexture_ = new RandomTexture;
	randomTexture_->Load(1000);

	InitUpdateShader();
	InitBillboardShader();

	return true;
}

void ParticleSystem::Render(float deltaTime, const glm::mat4& VP, const glm::vec3& cameraPosition) {
	time_ += deltaTime;

	UpdateParticles(deltaTime);
	RenderParticles(VP, cameraPosition);

	currentVB_ = currentTFB_;
	currentTFB_ = (1 + currentTFB_) & 0x1;
}

void ParticleSystem::InitUpdateShader() {
	updateShader_ = new Shader;
	updateShader_->Load(ShaderTypeVertex, "shaders/particle_system_update.vert");
	updateShader_->Load(ShaderTypeGeometry, "shaders/particle_system_update.geom");
	updateShader_->Load(ShaderTypeFragment, "shaders/particle_system_update.frag");

	const GLchar* varyings[] = {
		"type1", "position1", "velocity1", "age1",
	};

	// To specify the attributes that go into the feedback buffer we have to call glTransformFeedbackVaryings()
	// *before the technique program is linked*.
	// The strings in the array must contain names of output attributes from the last shader before the FS (either VS or GS).
	// When transform feedback is active these attributes will be written into the buffer per vertex.
	// The last parameter to glTransformFeedbackVaryings() tells OpenGL either to write all the attributes
	// as a single structure into a single buffer (GL_INTERLEAVED_ATTRIBS). 
	// Or to dedicate a single buffer for each attribute (GL_SEPARATE_ATTRIBS). 
	// If you use GL_INTERLEAVED_ATTRIBS you can only have a single transform feedback buffer bound (as we do).
	// If you use GL_SEPARATE_ATTRIBS you will need to bind a different buffer to each slot (according to the number of attributes). 
	// Remember that the slot is specified as the second parameter to glBindBufferBase().
	// In addition, you are limited to no more than GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS attribute slots (which is usually 4).
	glTransformFeedbackVaryings(updateShader_->GetProgram(), COUNT_OF(varyings), varyings, GL_INTERLEAVED_ATTRIBS);

	updateShader_->Link();

	//glActiveTexture(GL_TEXTURE1);
	//randomTexture_->Use();
	//updateShader_->SetUniform("randomTexture", 1);

	updateShader_->SetUniform("launcherLifeTime", 0.1f);
	updateShader_->SetUniform("shellLifeTime", 10.f);
	updateShader_->SetUniform("secondaryShellLifeTime", 2.5f);
}

void ParticleSystem::InitBillboardShader() {
	billboardShader_ = new Shader;
	billboardShader_->Load(ShaderTypeVertex, "shaders/billboard2.vert");
	billboardShader_->Load(ShaderTypeGeometry, "shaders/billboard2.geom");
	billboardShader_->Load(ShaderTypeFragment, "shaders/billboard2.frag");

	billboardShader_->Link();

	billboardShader_->SetUniform("discardOption", 1);
}

void ParticleSystem::UpdateParticles(float deltaTime) {
	updateShader_->Use();
	updateShader_->SetUniform("time", time_);
	updateShader_->SetUniform("deltaTime", deltaTime);

	// Tells the pipeline to discard all primitives before they reach the rasterizer(but after the 
	// optional transform feedback stage).
	glEnable(GL_RASTERIZER_DISCARD);

	// 'currentVB_' is used as an index (either 0 or 1) into the array of VBs and we bind the buffer 
	// in that slot as a vertex buffer (for input).
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer_[currentVB_]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback_[currentTFB_]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	// stride:
	// Specifies the byte offset between consecutive generic vertex attributes.
	// If stride is 0, the generic vertex attributes are understood to be tightly packed in the array. The initial value is 0.
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const void*)4);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const void*)16);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const void*)28);

	// glBeginTransformFeedback() makes transform feedback active. 
	// All the draw calls after that, and until glEndTransformFeedback() is called,
	// redirect their output to the transform feedback buffer according to the currently bound transform feedback object.
	// The way transform feedback works is that only complete primitives (i.e. lists) can be written into the buffer.
	glBeginTransformFeedback(GL_POINTS);

	// Since we generate and destroy particles based on the launcher frequency and each particle lifetime,
	// we cannot tell the draw call how many particles to process. This is all true - except for the very first draw.
	// In this case we know that our vertex buffer contains only the launcher and the "system" doesn't have any record 
	// of previous transform feedback activity so it cannot tell the number of particles on its own. 
	if (isFirst_) {
		glDrawArrays(GL_POINTS, 0, 1);
		isFirst_ = false;
	}
	else {
		// This function doesn't need to be told how many vertices to process. 
		// It simply checks the input buffer and draws all the vertices that have been previously written into it
		// (when it was bound as a transform feedback buffer). 
		// Note that whenever we bind a transform feedback object the number of vertices in the buffer becomes zero 
		// because we called glBindBufferBase() on that buffer.
		// The number of vertices to process as input comes from the transform feedback object which was bound as a target 
		// in the previous time we went through ParticleSystem::UpdateParticles().
		glDrawTransformFeedback(GL_POINTS, transformFeedback_[currentVB_]);
	}

	glEndTransformFeedback();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

void ParticleSystem::RenderParticles(const glm::mat4& VP, const glm::vec3& cameraPosition) {
	billboardShader_->Use();
	billboardShader_->SetUniform("cameraPosition", &cameraPosition);
	billboardShader_->SetUniform("VP", &VP);

	glActiveTexture(GL_TEXTURE0);
	texture_->Use();
	billboardShader_->SetUniform("textureSampler", 0);

	glActiveTexture(GL_TEXTURE1);
	randomTexture_->Use();
	updateShader_->SetUniform("randomTexture", 1);

	glDisable(GL_RASTERIZER_DISCARD);

	// When we wrote into the transform feedback buffer we bound m_transformFeedback[m_currTFB] as the transform feedback object(the target).
	// That object has m_particleBuffer[m_currTFB] as the attached vertex buffer. 
	// We now bind this buffer to provide the input vertices for rendering.
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer_[currentTFB_]);
	GLfloat* buffer = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const void*)4);
	// The second parameter is the transform feedback object that matches the input vertex buffer. 
	// This object "knows" how many vertices to draw.
	glDrawTransformFeedback(GL_POINTS, transformFeedback_[currentTFB_]);
	glDisableVertexAttribArray(0);
}
