#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "SceneCamera.h"
#include "Struct.h"
#include "Texture.h"

using namespace Elite;
class Object
{
public:
	Object(Texture* texture);
	Object(const Object& rhs) = delete;
	Object(Object&& other) = delete;
	Object& operator=(const Object& rhs) = delete;
	Object& operator=(const Object&& other) = delete;
	~Object();

	virtual void Update(float elapsedSec) = 0;
	virtual const bool Intersect(const FPoint2& pixel, float& depthZ, RGBColor& color, Vertex& V0, Vertex& V1, Vertex& V2, FVector3& n, int idx, const FVector3& viewDirection) const = 0;
	virtual const std::vector<Vertex>& GetWorldVertices() const = 0;
	virtual const std::vector<unsigned int>& GetIndexBuffers() const = 0;
	const Texture* GetTexture() const;
private:
	Texture* m_Texture = nullptr;
};
