#include "app.h"
#include "example.h"

int main(void)
{
	App app;
	if (!app.Initialize()) {
		return -1;
	}

	Example* example = new Example_TexturedCube;
	app.SetExample(example);

	app.EnterMainLoop();

	delete example;
	app.Destroy();

	return 0;
}
