#include "Object.h"

Object::Object(Texture* texture)
	:m_Texture{ texture }
{
}

Object::~Object()
{
	delete m_Texture;
	m_Texture = nullptr;
}

const Texture* Object::GetTexture() const
{
	return m_Texture;
}
