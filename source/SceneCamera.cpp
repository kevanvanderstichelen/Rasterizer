#include "SceneCamera.h"
#include <iostream>

SceneCamera::SceneCamera(const FVector3& position, float FOV, float sWidth, float sHeight)
	:m_ScreenWidth{ float(sWidth) }
	,m_ScreenHeight{ float(sHeight) }
	,m_AspectRatio{ float(sWidth) / float(sHeight) }
	,m_FOV{ tanf(ToRadians(FOV) / 2)}
	,m_LeftMouseButton{}
	,m_RightMouseButton{}
	,m_Matrix{}
	,m_MouseSensivity{0.06f}
	,m_TranslateRate{ 2.f }
	,m_Far{ 100.0f }
	,m_Near{ 0.1f }
{
	const FVector3 worldUp = { 0.f, 1.f, 0.f };
	const FVector3 forward = { 0.f, 0.f, 1.f };
	const FVector3 right = GetNormalized(Cross(worldUp, forward));
	const FVector3 up = Cross(forward, right);
	m_Matrix = { FVector4{right, 0.f}, FVector4{up, 0.f}, FVector4{forward, 0.f}, FVector4{position, 1.f} };
	m_ProjectionMatrix = { FVector4{1.f / (m_AspectRatio * m_FOV), 0, 0, 0},
						   FVector4{0, 1.f / m_FOV, 0, 0},
						   FVector4{ 0,0, m_Far / (m_Near - m_Far), -1},
						   FVector4{0,0,(m_Far * m_Near) / (m_Near - m_Far),0} };

}

void SceneCamera::HandleMouse(const float elapsedSec)
{
	int mouseX{};
	int mouseY{};
	uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

	if (mouseState & SDL_BUTTON(1) && mouseState & SDL_BUTTON(3))
	{
		m_Matrix *= MakeTranslation(FVector3(0, float(-mouseY) * elapsedSec * m_TranslateRate, 0));
	}
	else if (mouseState & SDL_BUTTON(1))
	{
		m_Matrix *= MakeTranslation(FVector3(0, 0, float(mouseY) * elapsedSec * m_TranslateRate));
		m_Matrix *= FMatrix4(MakeRotationY(float(mouseX) * elapsedSec * m_MouseSensivity));
	}
	else if (mouseState & SDL_BUTTON(3))
	{
		m_Matrix *= FMatrix4(MakeRotationX(float(mouseY) * elapsedSec * m_MouseSensivity));
		m_Matrix *= FMatrix4(MakeRotationY(float(mouseX) * elapsedSec * m_MouseSensivity));
	}
}

const FMatrix4 SceneCamera::GetViewProjectionMatrix() const
{
	return (m_ProjectionMatrix * Inverse(m_Matrix));
}

const FVector3 SceneCamera::GetDirection() const
{
	return GetNormalized(FVector3(m_Matrix[0][0], m_Matrix[1][1], m_Matrix[2][2]));
}

