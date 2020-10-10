#pragma once
#include "Object.h"
#include "Texture.h"
#include <regex>
#include <fstream>
#include <iostream>
#include <sstream>
class TriangleMesh : public Object
{
public:
	TriangleMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indexBuffer, Texture* texture, const float rotationSpeed);
	~TriangleMesh() = default;

	virtual const bool Intersect(const FPoint2& pixel, float& depthZ, RGBColor& color, Vertex& V0, Vertex& V1, Vertex& V2, FVector3& n, int idx, const FVector3& viewDirection) const override;
	virtual void Update(float elapsedSec) override;
	virtual const std::vector<Vertex>& GetWorldVertices() const override;
	virtual const std::vector<unsigned int>& GetIndexBuffers() const override;
	static TriangleMesh* LoadFromFile(const std::string& fileName, const FVector3& pos, Texture* texture, const float rotationSpeed);

private:
	const float m_RotationSpeed;
	const std::vector<unsigned int> m_IndexBuffer;
	std::vector<Vertex> m_WorldVertices;
	std::vector<float> m_SignedArea;



};

