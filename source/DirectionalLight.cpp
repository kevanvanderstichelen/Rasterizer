#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const FVector3& normal, const RGBColor& color, float intensity)
	:m_Direction{ GetNormalized(normal) }
	,m_Color{ color }
	,m_Intensity{ intensity }
{
}

const RGBColor DirectionalLight::CalculateShading(const FVector3& normal, const RGBColor& diffuse) const
{
	const float observedArea = Dot(-normal, m_Direction);
	if (observedArea < 0.f) return RGBColor{ 0, 0, 0 };
	return (m_Color * m_Intensity * diffuse * observedArea);
}

const FVector3& DirectionalLight::GetDirection() const
{
	return m_Direction;
}
