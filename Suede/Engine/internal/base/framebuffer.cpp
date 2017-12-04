#include "debug.h"
#include "framebuffer.h"
#include "tools/string.h"
#include "internal/memory/memory.h"
#include "internal/memory/factory.h"
#include "internal/base/textureinternal.h"

Framebuffer0::Framebuffer0() : oldFramebuffer_(0) {
}

void Framebuffer0::Create(int width, int height) {
	width_ = width, height_ = height;
	fbo_ = 0;
}

void Framebuffer0::Bind() {
	Clear();
	BindFramebuffer();
	BindViewport();
}

void Framebuffer0::Unbind() {
	UnbindFramebuffer();
	UnbindViewport();
}

void Framebuffer0::ReadBuffer(std::vector<uchar>& pixels) {
	BindFramebuffer();
	
	int oldPackAlignment = 4;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	pixels.resize(4 * GetWidth() * GetHeight());
	glReadPixels(0, 0, GetWidth(), GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackAlignment);
	UnbindFramebuffer();
}

void Framebuffer0::BindFramebuffer() {
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFramebuffer_);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
}

void Framebuffer0::UnbindFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, oldFramebuffer_);
	oldFramebuffer_ = 0;
}

void Framebuffer0::BindViewport() {
	glViewport(0, 0, GetWidth(), GetHeight());
}

void Framebuffer0::UnbindViewport() {
}

void Framebuffer0::Resize(int w, int h) {
	if (width_ == w && height_ == h) { return; }
	width_ = w;
	height_ = h;
}

void Framebuffer0::Clear(int buffers) {
	BindFramebuffer();
	glClearColor(clearColor_.r, clearColor_.g, clearColor_.b, 1);
	glClear(buffers);
	UnbindFramebuffer();
}

Framebuffer::Framebuffer() : depthRenderbuffer_(0), renderTextures_(nullptr), attachments_(nullptr) {
}

Framebuffer::~Framebuffer() {
	MEMORY_RELEASE_ARRAY(renderTextures_);
	MEMORY_RELEASE_ARRAY(attachments_);

	if (depthRenderbuffer_ != 0) {
		glDeleteRenderbuffers(1, &depthRenderbuffer_);
		depthRenderbuffer_ = 0;
	}
}

void Framebuffer::Create(int width, int height) {
	if (renderTextures_ != nullptr || attachments_ != nullptr) {
		Debug::LogError("framebuffer already created");
		return;
	}

	Framebuffer0::Create(width, height);

	attachedRenderTextureCount_ = 0;

	glGenFramebuffers(1, &fbo_);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxRenderTextures_);
	renderTextures_ = MEMORY_CREATE_ARRAY(RenderTexture, maxRenderTextures_);
	attachments_ = MEMORY_CREATE_ARRAY(GLenum, maxRenderTextures_);
}

void Framebuffer::Bind() {
	Framebuffer0::Bind();

	int count = UpdateAttachments();
	glDrawBuffers(count, attachments_);
}

int Framebuffer::UpdateAttachments() {
	if (attachedRenderTextureCount_ == 0) {
		attachments_[0] = GL_NONE;
		return 1;
	}

	int count = 0;
	for (int i = 0; i < maxRenderTextures_; ++i) {
		if (renderTextures_[i]) {
			attachments_[count++] = GL_COLOR_ATTACHMENT0 + i;
		}
	}

	return count;
}

void Framebuffer::CreateDepthRenderBuffer() {
	if (depthRenderbuffer_ != 0) {
		Debug::LogError("depth texture or render buffer already exists");
		return;
	}

	BindFramebuffer();

	glGenRenderbuffers(1, &depthRenderbuffer_);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GetWidth(), GetHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer_);

	UnbindFramebuffer();
}

void Framebuffer::SetDepthTexture(RenderTexture texture) {
	BindFramebuffer();
	depthTexture_ = texture;
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture ? texture->GetNativePointer() : 0, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		Debug::LogError("failed to bind depth texture");
	}

	UnbindFramebuffer();
}

int Framebuffer::GetRenderTextureCount() {
	return attachedRenderTextureCount_;
}

RenderTexture Framebuffer::GetDepthTexture() {
	return depthTexture_;
}

void Framebuffer::SetRenderTexture(RenderTexture texture) {
	BindFramebuffer();

	renderTextures_[0] = texture;

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture ? texture->GetNativePointer() : 0, 0);

	attachedRenderTextureCount_ = texture ? 1 : 0;
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		Debug::LogError("failed to create framebuffer, 0x04x.", status);
	}

	UnbindFramebuffer();
}

RenderTexture Framebuffer::GetRenderTexture() {
	return renderTextures_[0];
}

int Framebuffer::FindAttachmentIndex() {
	for (int i = 0; i < maxRenderTextures_; ++i) {
		if (!renderTextures_[i]) {
			return i;
		}
	}

	return -1;
}
