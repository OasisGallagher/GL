#include "app.h"
#include "example.h"

App app;

int main() {
	if (!app.Initialize()) {
		return -1;
	}

	Example* example = new Example_LightMaps;
	app.SetExample(example);

	app.EnterMainLoop();

	delete example;
	app.Destroy();

	return 0;
}