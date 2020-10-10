//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "ERGBColor.h"
#include "SceneGraph.h"
#include "LightManager.h"
#include <algorithm>

Elite::Renderer::Renderer(SDL_Window * pWindow)
	:m_ColorToggle{ true }
{
	//Initialize
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	//INITIALIZE DEPTH BUFFER (DEPTH CHECKER)
	m_DepthBuffer = new float* [height];
	for (int i{}; i < height; ++i) m_DepthBuffer[i] = new float[width];
}

Elite::Renderer::~Renderer()
{
	for (uint32_t i{}; i < m_Height; ++i) delete[] m_DepthBuffer[i];
	delete[] m_DepthBuffer;
}

void Elite::Renderer::Render(SceneCamera* pCamera)
{
	RGBColor color{ 0.1f,0.1f,0.1f };
	SDL_LockSurface(m_pBackBuffer);
	//Reseting Frame & Depth buffers to given parameters
	FlushDepthBuffer(FLT_MAX);
	FlushFrameBuffer(color);

	//Get view Projection matrix from camera
	const FMatrix4 worldViewProjectionMatrix{pCamera->GetViewProjectionMatrix()};
	
	//For every object in the scene 
	for (Object* obj : SceneGraph::GetObjectsFromGraph())
	{
		//Temporary variables
		float depthZ{};
		std::vector<Vertex> vertices;
		const std::vector<unsigned int>& indexBuffer = obj->GetIndexBuffers();

		//Project all vertices with ViewProjMatrix & Precalculate signed area's of the triangles V0,V1,V2
		VertexProjection(worldViewProjectionMatrix, obj->GetWorldVertices(), vertices, indexBuffer);

		for (int i = 0; i < vertices.size(); i += 3)
		{
			FPoint2 boxMin = { INFINITY, INFINITY };
			FPoint2 boxMax = { -INFINITY, -INFINITY };
			
			//Assign vertices V0, V1, V2
			Vertex& V0 = vertices[indexBuffer[i]];
			Vertex& V1 = vertices[indexBuffer[i + 1]];
			Vertex& V2 = vertices[indexBuffer[i + 2]];

			//Getting bounding box in raster space of V0,V1,V2
			GetBoundingBox(boxMin, boxMax, V0, V1, V2);

			//Bounding box determines where to check for intersection (pixel in triangle?)
			//Margin to prevent float precision issues + 1
			for (int r{ int(boxMin.y) }; r < int(boxMax.y) + 1; ++r)
			{
				for (int c{ int(boxMin.x) }; c < int(boxMax.x) + 1; ++c)
				{
					FVector3 normal{};
					const FPoint2 pixel{ float(c),float(r) };

					//Check if the pixel is inside the triangle
					if (obj->Intersect(pixel, depthZ, color, V0 ,V1 ,V2, normal, i, pCamera->GetDirection()))
					{
						//The most nearest pixel checked to be projected on screen must be between 0 - 1 otherwise
						//its outside the given frustum (near / far) from the camera
						if (depthZ < m_DepthBuffer[r][c] && depthZ >= 0 && depthZ <= 1)
						{
							m_DepthBuffer[r][c] = depthZ;
							//Switching between texture color / Depth color
							m_ColorToggle ? color = PixelShading(normal, color) : 
								color = RGBColor{ Remap(depthZ, 1.f, 0.985f), Remap(depthZ, 1.f, 0.985f), Remap(depthZ, 1.f, 0.985f) };
							
							//Color the screen
							m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
								static_cast<uint8_t>(color.r * 255.f),
								static_cast<uint8_t>(color.g * 255.f),
								static_cast<uint8_t>(color.b * 255.f));
						}
					}
				}
			}

		}
	}


	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Elite::Renderer::HandleControls()
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	if (pStates[SDL_SCANCODE_R]) m_ColorToggle = !m_ColorToggle;
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::FlushDepthBuffer(const float& element)
{
	for (uint32_t r{}; r < m_Height; ++r)
	{
		for (uint32_t c{}; c < m_Width; ++c)
		{
			m_DepthBuffer[r][c] = element;
		}
	}
}

void Elite::Renderer::FlushFrameBuffer(const RGBColor& color)
{
	for (uint32_t r{}; r < m_Height; ++r)
	{
		for (uint32_t c{}; c < m_Width; ++c)
		{
			m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(color.r * 255.f),
				static_cast<uint8_t>(color.g * 255.f),
				static_cast<uint8_t>(color.b * 255.f));
		}
	}
}

const void Elite::Renderer::VertexProjection(const FMatrix4& WVPmatrix, const std::vector<Vertex>& wVertices, std::vector<Vertex>& lVertices, const std::vector<unsigned int>& indicies) const
{
	//Projection stage, multiply World View Projection matrix with all vertices (w component for uv interpolation)

	for (int i{}; i < indicies.size(); i += 3)
	{
		Vertex V0 = wVertices[i];
		Vertex V1 = wVertices[i + 1];
		Vertex V2 = wVertices[i + 2];
		V0.pos = WVPmatrix * V0.pos;
		V1.pos = WVPmatrix * V1.pos;
		V2.pos = WVPmatrix * V2.pos;
		V0.pos.xyz /= V0.pos.w;
		V1.pos.xyz /= V1.pos.w;
		V2.pos.xyz /= V2.pos.w;

		if (V0.pos.x < -1.f || V0.pos.x > 1.f || V0.pos.y < -1.f || V0.pos.y > 1.f) continue;
		if (V1.pos.x < -1.f || V1.pos.x > 1.f || V1.pos.y < -1.f || V1.pos.y > 1.f) continue;
		if (V2.pos.x < -1.f || V2.pos.x > 1.f || V2.pos.y < -1.f || V2.pos.y > 1.f) continue;
		V0.pos = VertexToRaster(V0.pos);
		V1.pos = VertexToRaster(V1.pos);
		V2.pos = VertexToRaster(V2.pos);
		lVertices.push_back(V0);
		lVertices.push_back(V1);
		lVertices.push_back(V2);
	}
}

const FPoint4 Elite::Renderer::VertexToRaster(const FPoint4& vertex) const
{
	//Rasterization stage, From projection stage -> Screen Space
	return FPoint4{ ((vertex.x + 1.f) / 2.f) * m_Width, ((1.f - vertex.y) / 2.f) * m_Height, vertex.z, vertex.w };
}

const void Elite::Renderer::GetBoundingBox(FPoint2& min, FPoint2& max, const Vertex& v0, const Vertex& v1, const Vertex& v2) const
{
	//Getting minimum or max X,Y of the vertices
	GetVertexMinMax(min, max, v0);
	GetVertexMinMax(min, max, v1);
	GetVertexMinMax(min, max, v2);

	//Clamp between the screen Height/Width
	min.x = Elite::Clamp(min.x, 0.f, static_cast<float>(m_Width));
	min.y = Elite::Clamp(min.y, 0.f, static_cast<float>(m_Height));
	max.x = Elite::Clamp(max.x, 0.f, static_cast<float>(m_Width));
	max.y = Elite::Clamp(max.y, 0.f, static_cast<float>(m_Height));
}

const void Elite::Renderer::GetVertexMinMax(FPoint2& min, FPoint2& max, const Vertex& v) const
{
	if (v.pos.x < min.x) min.x = v.pos.x;
	if (v.pos.y < min.y) min.y = v.pos.y;
	if (v.pos.x > max.x) max.x = v.pos.x;
	if (v.pos.y > max.y) max.y = v.pos.y;
}

const RGBColor Elite::Renderer::PixelShading(const FVector3& normal, const RGBColor& textureColor) const
{
	//Final pixel shading with light color
	RGBColor finalColor{};
	for (const Lights* light : LightManager::GetLightsFromGraph())
	{
		finalColor += light->CalculateShading(normal, textureColor);
	}
	finalColor.MaxToOne();
	return finalColor;
}
 