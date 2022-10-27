#include <Cliffine.h>

using namespace Clf;

App* App::Create()
{
	auto app = (App*)Mem::Alloc(Mem::MEM_TYPE_APP, sizeof(App));
	app->name = "Cliffine";
	app->x = 100;
	app->y = 100;
	app->width = 1200;
	app->height = 720;

	return app;
}