#pragma once
#include "Lights.h"
using namespace Elite;
class DirectionalLight : public Lights
{
public:
	DirectionalLight(const FVector3& normal, const RGBColor& color, float intensity);
	~DirectionalLight() = default;
	virtual const RGBColor CalculateShading(const FVector3& normal, const RGBColor& diffuse) const override;
	virtual const FVector3& GetDirection() const override;
private:
	const FVector3 m_Direction;
	const RGBColor m_Color;
	const float m_Intensity;

};