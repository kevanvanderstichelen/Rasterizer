#include "Texture.h"
#include "Lights.h"
#include "LightManager.h"

Texture::Texture(const std::string& fullFileNameDiffuse)
	:m_Shineyness{}
{
	m_Diffuse = IMG_Load(fullFileNameDiffuse.c_str());
}

//! WARNING: PREFIX + [ ROOT ] + SUFFIX, ROOT DECLARED IN CONSTRUCTOR !
Texture::Texture(const std::string& fileName, const std::string& fileType, const float shineyness)
	:m_RootDiffuseName{ "_diffuse" }
	,m_RootNormalName{ "_normal" }
	,m_RootSpecularName{"_specular"}
	,m_RootGlossName{"_gloss"}
	,m_Shineyness{ shineyness }
{
	std::string diffuseImage = fileName + m_RootDiffuseName + fileType;
	std::string normalImage = fileName + m_RootNormalName + fileType;
	std::string glossImage = fileName + m_RootGlossName + fileType;
	std::string specularImage = fileName + m_RootSpecularName + fileType;
	m_Diffuse = IMG_Load(diffuseImage.c_str());
	m_Normal = IMG_Load(normalImage.c_str());
	m_Gloss = IMG_Load(glossImage.c_str());
	m_Specular = IMG_Load(specularImage.c_str());
}

Texture::~Texture()
{
	SDL_FreeSurface(m_Diffuse);
	SDL_FreeSurface(m_Normal);
	SDL_FreeSurface(m_Gloss);
	SDL_FreeSurface(m_Specular);
}

const RGBColor Texture::Sample(const FVector2& uv) const
{
	if (m_Diffuse == nullptr) return {};

	//If interpolated is out of range (z axis) cancel sample
	if (uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1)
	{
		Uint8 r, g, b;
		const FVector2 UV = { uv.x * m_Diffuse->w, m_Diffuse->h - (uv.y * m_Diffuse->h) };
		const uint32_t pixel = *((uint32_t*)m_Diffuse->pixels + uint32_t(UV.y) * m_Diffuse->w + uint32_t(UV.x));

		//Fetch color in UV of that pixel
		SDL_GetRGB(pixel, m_Diffuse->format, &r, &g, &b);

		return RGBColor{ float(r) / 255.f , float(g) / 255.f, float(b) / 255.f };
	}
	else return {};
}

//There must be an Gloss & Specular & Normal map for using this function
const RGBColor Texture::Sample(const FVector2& uv, FVector3& normal, const FVector3& tangent, const FVector3& viewDirection) const
{
	if (m_Diffuse == nullptr || m_Normal == nullptr || m_Gloss == nullptr || m_Specular == nullptr) return {};

	if (uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1)
	{
		Uint8 r, g, b;

		//Sample from diffuse/albedo color
		FVector2 UV = { uv.x * m_Diffuse->w, m_Diffuse->h - (uv.y * m_Diffuse->h) };
		uint32_t pixel = *((uint32_t*)m_Diffuse->pixels + uint32_t(UV.y) * m_Diffuse->w + uint32_t(UV.x));
		SDL_GetRGB(pixel, m_Diffuse->format, &r, &g, &b);
		const RGBColor uvColor = RGBColor{ float(r) / 255.f , float(g) / 255.f, float(b) / 255.f };

		//Sample from normal texture
		pixel = *((uint32_t*)m_Normal->pixels + uint32_t(UV.y) * m_Normal->w + uint32_t(UV.x));
		SDL_GetRGB(pixel, m_Normal->format, &r, &g, &b);
		FVector3 normalValue = FVector3(float(r) / 255.f, float(g) / 255.f, float(b) / 255.f);

		normalValue.x = 2.f * normalValue.x - 1.f;
		normalValue.y = 2.f * normalValue.y - 1.f;
		normalValue.z = 2.f * normalValue.z - 1.f;

		//Sample from glossiness texture
		pixel = *((uint32_t*)m_Gloss->pixels + uint32_t(UV.y) * m_Gloss->w + uint32_t(UV.x));
		SDL_GetRGB(pixel, m_Gloss->format, &r, &g, &b);
		float glossValue = float(r) / 255.f;

		//Sample from specular texture
		pixel = *((uint32_t*)m_Specular->pixels + uint32_t(UV.y) * m_Specular->w + uint32_t(UV.x));
		SDL_GetRGB(pixel, m_Specular->format, &r, &g, &b);
		float specularValue = float(r) / 255.f;

		//Calculate the new normal applied with the sampled normal map
		const FVector3 binormal = Cross(tangent, normal);
		FMatrix3 tangetSpaceAxis = FMatrix3(tangent, binormal, normal);
		normal = GetNormalized(tangetSpaceAxis * normalValue);

		//Calculate phong
		RGBColor phong{};
		for (const Lights* light : LightManager::GetLightsFromGraph())
		{
			FVector3 lightDirection = light->GetDirection();
			const FVector3 reflect = -lightDirection + (2.f * (Dot(normal, lightDirection)) * normal);
			const float cosAngle = Dot(reflect, viewDirection);
			if (cosAngle < 0) continue;
			phong += RGBColor(specularValue, specularValue, specularValue) * powf(cosAngle, glossValue * m_Shineyness);
		}

		//Final unlighted color
		return uvColor + phong;
	}
	else return {};

}
