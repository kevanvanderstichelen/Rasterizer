/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render to, does traverse the pixels 
// and traces the rays using a tracer
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include "SceneCamera.h"
#include "ERGBColor.h"
#include "Struct.h"

struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(SceneCamera* pCamera);
		void HandleControls();
		bool SaveBackbufferToImage() const;

	private:
		SDL_Window* m_pWindow = nullptr;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		float** m_DepthBuffer;
		bool m_ColorToggle;

		void FlushDepthBuffer(const float& element);
		void FlushFrameBuffer(const RGBColor& color);
		const void VertexProjection(const FMatrix4& WVPmatrix, const std::vector<Vertex>& wVertices, std::vector<Vertex>& lVertices, const std::vector<unsigned int>& indicies) const;
		const FPoint4 VertexToRaster(const FPoint4& vertex) const;
		const void GetBoundingBox(FPoint2& min, FPoint2& max, const Vertex& v0, const Vertex& v1, const Vertex& v2) const;
		const void GetVertexMinMax(FPoint2& min, FPoint2& max, const Vertex& v) const;
		const RGBColor PixelShading(const FVector3& normal, const RGBColor& textureColor) const;


	};
}

#endif