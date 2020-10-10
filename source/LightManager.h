#pragma once
#include "SDL.h"
#include "Lights.h"
#include <vector>

using namespace Elite;
class LightManager
{
public:
	static LightManager* GetInstance();
	static void AddLightToGraph(Lights* light);
	static const std::vector<Lights*>& GetLightsFromGraph();
	static void ResetInstance();
private:
	static std::vector<Lights*> m_Lights;
	static LightManager* m_Instance;
	LightManager() = default;
	~LightManager();
};

