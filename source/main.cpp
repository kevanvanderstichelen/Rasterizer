//External includes
#include "vld.h"
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "SceneGraph.h"
#include "SceneCamera.h"
#include "Struct.h"
#include "TriangleMesh.h"
#include "LightManager.h"
#include "DirectionalLight.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 1920;
	const uint32_t height = 1080;
	SDL_Window* pWindow = SDL_CreateWindow(
		"Rasterizer - Kevan Vanderstichelen",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	Elite::Timer* pTimer = new Elite::Timer();
	Elite::Renderer* pRenderer = new Elite::Renderer(pWindow);
	SceneCamera* pCamera = new SceneCamera(FVector3{ 0,5,50 }, 60.f, width, height);
	SceneGraph::GetInstance()->AddObjectToGraph(TriangleMesh::LoadFromFile("vehicle.obj", FVector3(0, 0, 0), new Texture("vehicle", ".png", 15.f), 0.2f));
	LightManager::GetInstance()->AddLightToGraph(new DirectionalLight(FVector3(.577f, -.577f, -.577f), RGBColor(242.f / 255.f, 247.f / 255.f, 255.f / 255.f), 2.f));
	
	//Extra info
	std::cout << "[R] to see depth buffer" << std::endl;
	std::cout << "[RMB] to rotate camera" << std::endl;
	std::cout << "[LMB] to rotate & move camera" << std::endl;
	std::cout << "[RMB] & [LMB] to move up and down ( local axis )" << std::endl;

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if(e.key.keysym.scancode == SDL_SCANCODE_X)
					takeScreenshot = true;
				break;
			}
		}
		//--------- Update ---------
		for (Object* obj : SceneGraph::GetInstance()->GetObjectsFromGraph()) obj->Update(pTimer->GetElapsed());
		

		//--------- Handle input events ---------
		pCamera->HandleMouse(pTimer->GetElapsed());
		pRenderer->HandleControls();

		//--------- Render ---------
		pRenderer->Render(pCamera);

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	SceneGraph::ResetInstance();
	LightManager::ResetInstance();
	delete pRenderer;
	delete pTimer;
	delete pCamera;

	ShutDown(pWindow);
	return 0;
}