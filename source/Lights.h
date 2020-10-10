#pragma once
#include "ERGBColor.h"
#include "EMath.h"
using namespace Elite;
class Lights
{
public:
	Lights() = default;
	~Lights() = default;
	virtual const RGBColor CalculateShading(const FVector3& normal, const RGBColor& diffuse) const = 0;
	virtual const FVector3& GetDirection() const = 0;
};

