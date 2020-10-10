#include "LightManager.h"

using namespace Elite;
LightManager* LightManager::m_Instance{ nullptr };
std::vector<Lights*> LightManager::m_Lights{};

LightManager* LightManager::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new LightManager{};
	}
	return m_Instance;
}

void LightManager::AddLightToGraph(Lights* light)
{
	m_Lights.push_back(light);
}

const std::vector<Lights*>& LightManager::GetLightsFromGraph()
{
	return m_Lights;
}

void LightManager::ResetInstance()
{
	delete m_Instance;
	m_Instance = nullptr;
}

LightManager::~LightManager()
{
	for (Lights* light : m_Lights)
	{
		delete light;
		light = nullptr;
	}
	m_Lights.clear();
}
