#include "SceneGraph.h"

using namespace Elite;
SceneGraph* SceneGraph::m_Instance{ nullptr };
std::vector<Object*> SceneGraph::m_Objects{};

SceneGraph::~SceneGraph()
{
	for (Object* obj : m_Objects)
	{
		delete obj;
		obj = nullptr;
	}
	m_Objects.clear();
}

SceneGraph* SceneGraph::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new SceneGraph{};
	}
	return m_Instance;
}

void SceneGraph::AddObjectToGraph(Object* object)
{
	m_Objects.push_back(object);
}

const std::vector<Object*>& SceneGraph::GetObjectsFromGraph()
{
	return m_Objects;
}

void SceneGraph::ResetInstance()
{
	delete m_Instance;
	m_Instance = nullptr;
}
