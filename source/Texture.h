#pragma once
#include <SDL_image.h>
#include "EMath.h"
#include <vector>
#include <string>
#include "ERGBColor.h"

using namespace Elite;
class Texture
{
public:
	Texture(const std::string& fullFileName);
	Texture(const std::string& fileName, const std::string& fileType, const float shineyness);
	Texture(const Texture& rhs) = delete;
	Texture(Texture&& other) = delete;
	Texture& operator=(const Texture& rhs) = delete;
	Texture& operator=(const Texture&& other) = delete;
	~Texture();

	const RGBColor Sample(const FVector2& uv) const;
	const RGBColor Sample(const FVector2& uv, FVector3& normal, const FVector3& tangent, const FVector3& viewDirection) const;

private:
	const std::string m_RootDiffuseName;
	const std::string m_RootNormalName;
	const std::string m_RootGlossName;
	const std::string m_RootSpecularName;
	const float m_Shineyness;
	SDL_Surface* m_Diffuse = nullptr;
	SDL_Surface* m_Normal = nullptr;
	SDL_Surface* m_Gloss = nullptr;
	SDL_Surface* m_Specular = nullptr;
};

