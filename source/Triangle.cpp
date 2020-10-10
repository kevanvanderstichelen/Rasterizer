#include "Triangle.h"

Triangle::Triangle(const FVector3& pos, const Vertex& v0, const Vertex& v1, const Vertex& v2)
	:Object{ nullptr }
	,m_worldV0{ v0.vert.xyz + pos }
	, m_worldV1{ v1.vert.xyz + pos }
	, m_worldV2{ v2.vert.xyz + pos }
	, m_V0Color { v0.color }
	, m_V1Color { v1.color }
	, m_V2Color { v2.color }
	, m_Normal{ GetNormalized(Cross(m_worldV1 - m_worldV0, m_worldV2 - m_worldV0)) }
	, m_Center{ (m_worldV0 + m_worldV1 + m_worldV2) / 3.f }
	, m_Position{ pos }
	, m_SignedArea{ }
{

}
const bool Triangle::Intersect(const FPoint2& pixel, float& depthZ, RGBColor& color, Vertex& V0, Vertex& V1, Vertex& V2, FVector3& n, int idx) const
{
	//CHECK IF PIXEL IS IN TRIANGLE
	FVector2 pointToSide = pixel - V0.vert.xy;
	FVector2 edge = V1.vert.xy - V0.vert.xy;
	float crossPointEdge = Cross(pointToSide, edge);
	if (crossPointEdge < 0.f) return false;
	const float W2 = crossPointEdge / m_SignedArea;

	pointToSide = pixel - V1.vert.xy;
	edge = V2.vert.xy - V1.vert.xy;
	crossPointEdge = Cross(pointToSide, edge);
	if (crossPointEdge < 0.f) return false;
	const float W0 = crossPointEdge / m_SignedArea;

	pointToSide = pixel - V2.vert.xy;
	edge = V0.vert.xy - V2.vert.xy;
	crossPointEdge = Cross(pointToSide, edge);
	if (crossPointEdge < 0.f) return false;
	const float W1 = crossPointEdge / m_SignedArea;

	depthZ = m_V0.z * W0 + m_V1.z * W1 + m_V2.z * W2;
	color = m_V0Color * W0 + m_V1Color * W1 + m_V2Color * W2;

	return true;
}

void Triangle::PreCalculateSignedArea(const std::vector<Vertex>& lVertices)
{
	//m_Normal = GetNormalized(Cross(m_V1 - m_V0, m_V2 - m_V0));
	//m_SignedArea = Cross(m_V2.xy - m_V0.xy, m_V1.xy - m_V0.xy);
}

void Triangle::Update(float elapsedSec)
{
}
