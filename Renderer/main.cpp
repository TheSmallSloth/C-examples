#include"Headers/ExternalHeaders.h"

#include "Physics/Physics3d.h"
#include "Renderer/Renderer3d.h"

#include "Headers/Window.h"
#include "Headers/GameObjects.h"
#include "Headers/Scenes.h"

#include <memory>


bool simThreadLooping = true;
bool simThreadReturned = false;
std::unique_ptr<Scene> currentScene;


DWORD WINAPI ThreadProc(LPVOID threadParam)
{
	while (simThreadLooping)
	{
		currentScene->updateSim(0.01f);
		Sleep(9);
	}
	simThreadReturned = true;
	return 0;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	Window window("Disco Disco", 1280, 720, 100, 100, hInstance, nCmdShow);

	InitializeFunctionPointers();

	// STBI_IMAGE: flip y-coordinates whilst loading
	stbi_set_flip_vertically_on_load(1);

	loadTextures();
	loadCubeStuff();

	Renderer::initialiseShaders();
	Renderer::fillBuffers();
	Renderer::generateSamplers();

	currentScene = std::make_unique<TestScene>(100, 100);


	MSG message;

	HANDLE simThreadHandle = CreateThread(NULL, NULL, ThreadProc, NULL, NULL, NULL);

	while (looping)
	{
		if (PeekMessage(&message, window.getWindowHandle(), NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		currentScene->update();

		currentScene->draw(Renderer::Main, Renderer::Shadow);

		window.update();
	}

	simThreadLooping = false;
	while (!simThreadReturned){}

	Renderer::cleanUp();

	return 0;
}