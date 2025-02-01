#include "App.h"

int main(void)
{
	App app;
	app.initialize();
	app.update_frame();
	app.close();

	return 0;
}