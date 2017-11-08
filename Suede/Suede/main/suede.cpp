#include <QMenuBar>
#include <QSplitter>
#include <QKeyEvent>
#include <QFileDialog>

#include "suede.h"

#include "views/game.h"
#include "views/canvas.h"
#include "views/console.h"
#include "views/inspector.h"
#include "views/hierarchy.h"

#include "camera.h"

Suede::Suede(QWidget *parent)
	: QMainWindow(parent) {
	Engine::get()->setLogReceiver(this);

	setupUI();

	QMenu* fileMenu = menuBar()->findChild<QMenu*>("file");
	QList<QAction*> actions = fileMenu->actions();

	connect(actions[0], SIGNAL(triggered()), this, SLOT(screenCapture()));
	connect(actions[1], SIGNAL(triggered()), qApp, SLOT(quit()));
}

Suede::~Suede() {
}

void Suede::setupUI() {
	ui.setupUi(this);

	QWidget* cw = takeCentralWidget();
	cw->deleteLater();

	dockWidgets_[ChildWindowGame] = ui.game;
	dockWidgets_[ChildWindowConsole] = ui.console;
	dockWidgets_[ChildWindowInspector] = ui.inspector;
	dockWidgets_[ChildWindowHierarchy] = ui.hierarchy;

	addDockWidget(Qt::LeftDockWidgetArea, ui.inspector);
	addDockWidget(Qt::RightDockWidgetArea, ui.game);
	addDockWidget(Qt::RightDockWidgetArea, ui.console, Qt::Vertical);
	addDockWidget(Qt::RightDockWidgetArea, ui.hierarchy, Qt::Horizontal);

	menuBar()->hide();
	for (int i = ChildWindowGame + 1; i < ChildWindowCount; ++i) {
		showChildWindow((ChildWindow)i, false);
	}

	Game::get()->setView(ui.gameWidget);
	Console::get()->setView(ui.consoleWidget);
	Inspector::get()->setView(ui.inspectorWidget);
	Hierarchy::get()->setView(ui.hierarchyWidget);
}

void Suede::showChildWindow(ChildWindow window, bool show) {
	Q_ASSERT(window > 0 && window < ChildWindowCount);
	dockWidgets_[window]->setVisible(show);
}

bool Suede::childWindowVisible(ChildWindow window) {
	Q_ASSERT(window > 0 && window < ChildWindowCount);
	return dockWidgets_[window]->isVisible();
}

void Suede::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
		case Qt::Key_Escape:
			onEscapePressed();
			break;

		case Qt::Key_0:
		case Qt::Key_1:
		case Qt::Key_2:
		case Qt::Key_3:
		case Qt::Key_4:
		case Qt::Key_5:
		case Qt::Key_6:
		case Qt::Key_7:
		case Qt::Key_8:
		case Qt::Key_9:
			onNumberPressed(event);
			break;
	}
}

void Suede::onNumberPressed(QKeyEvent* event) {
	if ((event->modifiers() & Qt::ControlModifier) == 0) {
		return;
	}

	int index = event->key() - Qt::Key_0;
	if (index > 0 && index < ChildWindowCount) {
		showChildWindow((ChildWindow)index, !childWindowVisible((ChildWindow)index));
	}
}

void Suede::onEscapePressed() {
	qApp->quit();
}

void Suede::screenCapture() {
	std::vector<Sprite> sprites;
	if (!Engine::get()->world()->GetSprites(ObjectTypeCamera, sprites)) {
		return;
	}

	Camera camera = dsp_cast<Camera>(sprites.front());

	Texture2D tex = camera->Capture();
	std::vector<unsigned char> data;
	if (!tex->EncodeToJpg(data)) {
		return;
	}

	QImage image;
	if (image.loadFromData(&data[0], data.size())) {
		QString filter = "image(*.jpg)";
		QString path = QFileDialog::getSaveFileName(this, "", "", filter);
		if (!path.isEmpty()) {
			image.save(path);
		}
	}
}

void Suede::OnEngineLogMessage(int type, const char* message) {
	if (!childWindowVisible(ChildWindowConsole)) {
		return;
	}

	switch (type) {
		case 0:
			Console::get()->addMessage(Console::Debug, message);
			break;

		case 1:
			Console::get()->addMessage(Console::Warning, message);
			break;

		case 2:
		case 3:
			Console::get()->addMessage(Console::Error, message);
			if (type == 3) { qFatal(message); }
			break;
	}
}
