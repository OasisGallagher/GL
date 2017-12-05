#include <QWidget>

#include "game.h"
#include "font.h"
#include "mesh.h"
#include "light.h"
#include "world.h"
#include "skybox.h"
#include "camera.h"
#include "shader.h"
#include "skybox.h"
#include "engine.h"
#include "texture.h"
#include "variables.h"
#include "particlesystem.h"

#include "scripts/grayscale.h"
#include "scripts/inversion.h"
#include "scripts/cameracontroller.h"

//#define SKYBOX
//#define MODEL
//#define POST_EFFECTS
#define ANIMATION
//#define PARTICLE_SYSTEM
//#define FONT
//#define BUMPED

Game* Game::get() {
	static Game instance;
	return &instance;
}

Game::Game() : sceneCreated_(false) {
	grayscale_ = new Grayscale;
	inversion_ = new Inversion;
	controller_ = new CameraController;
}

Game::~Game() {
	delete grayscale_;
	delete inversion_;
	delete controller_;
}

void Game::initialize() {
	canvas_ = view_->findChild<QWidget*>("canvas", Qt::FindDirectChildrenOnly);
	canvas_->installEventFilter(this);
	connect(canvas_, SIGNAL(repaint()), this, SLOT(update()), Qt::DirectConnection);
	view_->setSize(300, 400);
}

bool Game::eventFilter(QObject* watched, QEvent* event) {
	if (watched != canvas_) {
		return false;
	}

	switch (event->type()) {
	case QEvent::Wheel:
		wheelEvent((QWheelEvent*)event);
		break;
	case QEvent::MouseButtonPress:
		mousePressEvent((QMouseEvent*)event);
		break;
	case QEvent::MouseButtonRelease:
		mouseReleaseEvent((QMouseEvent*)event);
		break;
	case QEvent::MouseMove:
		mouseMoveEvent((QMouseEvent*)event);
		break;
	case QEvent::KeyRelease:
		keyPressEvent((QKeyEvent*)event);
		break;

	case QEvent::Resize:
		resizeEvent((QResizeEvent*)event);
		break;
	}

	return false;
}

void Game::wheelEvent(QWheelEvent* event) {
	controller_->onMouseWheel(event->delta());
}

void Game::mousePressEvent(QMouseEvent *event) {
	controller_->onMousePress(event->button(), event->pos());
}

void Game::mouseReleaseEvent(QMouseEvent* event) {
	controller_->onMouseRelease(event->button());
}

void Game::mouseMoveEvent(QMouseEvent *event) {
	controller_->onMouseMove(event->pos());
}

void Game::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
	}
}

void Game::resizeEvent(QResizeEvent* event) {
	controller_->onResize(event->size());
}

void Game::update() {
	if (!sceneCreated_) {
		createScene();
		sceneCreated_ = true;
	}
}

void Game::createScene() {
	WorldInstance()->GetEnvironment()->SetAmbientColor(glm::vec3(0.15f));
	DirectionalLight light = NewDirectionalLight();
	light->SetColor(glm::vec3(0.7f));

	Camera camera = NewCamera();
	controller_->setCamera(camera);
	camera->SetPosition(glm::vec3(0, 25, 0));

#ifdef POST_EFFECTS
	camera->AddPostEffect(inversion_);
	camera->AddPostEffect(grayscale_);
#endif

#ifdef SKYBOX
	camera->SetClearType(ClearTypeSkybox);
	Skybox skybox = NewSkybox();
	std::string faces[] = {
		"textures/lake_skybox/right.jpg",
		"textures/lake_skybox/left.jpg",
		"textures/lake_skybox/top.jpg",
		"textures/lake_skybox/bottom.jpg",
		"textures/lake_skybox/back.jpg",
		"textures/lake_skybox/front.jpg",
	};

	skybox->Load(faces);
	camera->SetSkybox(skybox);
#else
	camera->SetClearType(ClearTypeColor);
	camera->SetClearColor(glm::vec3(0, 0, 0.1f));
#endif
	
#ifdef RENDER_TEXTURE
	RenderTexture renderTexture = NewRenderTexture();
	renderTexture->Load(RenderTextureFormatRgba, canvas_->width(), canvas_->height());
	camera->SetRenderTexture(renderTexture);
#endif

#ifdef PARTICLE_SYSTEM
	ParticleSystem particleSystem = NewParticleSystem();
	particleSystem->SetPosition(glm::vec3(-30, 20, -50));

	SphereParticleEmitter emitter = NewSphereParticleEmitter();
	emitter->SetRadius(5);
	emitter->SetRate(200);
	emitter->SetStartColor(glm::vec4(1, 1, 1, 0.5f));
	emitter->SetStartDuration(2);
	emitter->SetStartSize(1);
	emitter->SetStartVelocity(glm::vec3(0, 1, 0));
	ParticleBurst burst = { 4, 3, 20 };
	particleSystem->SetEmitter(emitter);

	ParticleAnimator animator = NewParticleAnimator();
	animator->SetGravityScale(0.2f);
	particleSystem->SetParticleAnimator(animator);

	particleSystem->SetMaxParticles(1000);
	particleSystem->SetDuration(5);
	particleSystem->SetLooping(true);
#endif

#if defined(FONT)
	Sprite fsprite = NewSprite();
	fsprite->SetPosition(glm::vec3(-10, 20, -20));
	fsprite->SetEulerAngles(glm::vec3(0, 0, 0));

	Sprite fsprite2 = NewSprite();
	fsprite2->SetPosition(glm::vec3(-10, 30, -20));

	Font font = NewFont();
	font->Load("fonts/ms_yh.ttf", 12);

	TextMesh mesh = NewTextMesh();
	mesh->SetFont(font);
	mesh->SetText("落霞与孤鹜齐飞 秋水共长天一色");
	
	mesh->SetFontSize(12);
	fsprite->SetMesh(mesh);
	fsprite2->SetMesh(mesh);

	Renderer renderer = NewMeshRenderer();
	Material fontMaterial = dsp_cast<Material>(font->GetMaterial()->Clone());
	fontMaterial->SetVector4(Variables::mainColor, glm::vec4(1, 0, 0, 1));

	Renderer renderer2 = NewMeshRenderer();
	Material fontMaterial2 = dsp_cast<Material>(font->GetMaterial()->Clone());
	fontMaterial2->SetVector4(Variables::mainColor, glm::vec4(1, 1, 0, 1));

	renderer->AddMaterial(fontMaterial);
	renderer2->AddMaterial(fontMaterial2);

	fsprite->SetRenderer(renderer);
	fsprite2->SetRenderer(renderer2);
#endif

#if defined(MODEL) || defined(ANIMATION)
	Sprite sprite;
#if defined(MODEL)
	sprite = WorldInstance()->Import("models/jeep.fbx");
	sprite->SetPosition(glm::vec3(0, -20, -150));
#elif defined(ANIMATION)
	sprite = WorldInstance()->Import("models/boblampclean.md5mesh");
	sprite->SetPosition(glm::vec3(0, 0, -70));
	sprite->SetEulerAngles(glm::vec3(270, 0, 180));
#endif

	if (sprite) {
		sprite->SetParent(camera);
		light->SetParent(camera);

		Animation animation = sprite->GetAnimation();
		if (animation) {
			animation->SetWrapMode(AnimationWrapModePingPong);
			animation->Play("");
		}
	}

#endif
}
