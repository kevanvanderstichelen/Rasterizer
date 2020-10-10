#pragma once
#include "EMath.h"
#include "ERGBColor.h"

namespace Elite
{
	struct Vertex
	{
		FPoint4 pos = { 0, 0, 0, 0 };
		RGBColor color = { 1,1,1 };
		FVector2 uv;
		FVector3 normal;
		FVector3 tangent = { 0,0,0 };
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	struct ObjIndexBuffer
	{
		unsigned int vertex[3]{};
		unsigned int uv[3]{};
		unsigned int norm[3]{};
	};
}
