#pragma once
#include <gl/glew.h>

class RenderState {
public:
	static void PushFramebuffer(GLuint fbo) {
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&get().fbo_);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	static void PopFramebuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, get().fbo_);
	}

	static void PushViewport(GLint x, GLint y, GLsizei w, GLsizei h) {
		glGetIntegerv(GL_VIEWPORT, (GLint*)get().viewport_);
		glViewport(x, y, w, h);
	}

	static void PopViewport() {
		glViewport(get().viewport_[0], get().viewport_[1], get().viewport_[2], get().viewport_[3]);
	}

private:
	RenderState() {
	}

private:
	static RenderState& get() {
		static RenderState state;
		return state;
	}

private:
	GLuint fbo_;
	GLuint viewport_[4];
};
