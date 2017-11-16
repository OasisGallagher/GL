#pragma once

#include "object.h"
#include "surface.h"
#include "material.h"
#include "animation.h"

enum RenderQueue {
	RenderQueueBackground = 1000,
	RenderQueueGeometry = 2000,
	RenderQueueTransparent = 4000,
};

DEFINE_OBJECT_PTR(Sprite);

class ENGINE_EXPORT IRenderer : virtual public IObject {
public:
	virtual void RenderSprite(Sprite sprite) = 0;

	/**
	 * @brief Render surface with current bound material.
	 */
	virtual void RenderSurface(Surface surface) = 0;
	
	virtual void AddMaterial(Material material) = 0;
	virtual Material GetMaterial(int index) = 0;
	virtual void SetMaterial(int index, Material value) = 0;
	virtual int GetMaterialCount() = 0;

	virtual void SetRenderQueue(int value) = 0;
	virtual int GetRenderQueue() = 0;
};

class ENGINE_EXPORT ISurfaceRenderer : virtual public IRenderer {

};

class ENGINE_EXPORT IParticleRenderer : virtual public IRenderer {
};

class ENGINE_EXPORT ISkinnedSurfaceRenderer : virtual public IRenderer {
public:
	virtual void SetSkeleton(Skeleton value) = 0;
};

DEFINE_OBJECT_PTR(Renderer);
DEFINE_OBJECT_PTR(SurfaceRenderer);
DEFINE_OBJECT_PTR(ParticleRenderer);
DEFINE_OBJECT_PTR(SkinnedSurfaceRenderer);
