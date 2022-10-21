#pragma once

extern clf::App* clf::CreateApp();

int main()
{
	clf::Log();
	clf::App* app = clf::CreateApp();
	app->Run();
	delete app;
}