#pragma once
#include "EMath.h"
#include "SDL.h"
#include <vector>
using namespace Elite;
class SceneCamera
{
public:
	SceneCamera(const FVector3& pos, const float FOV, const float sWidth, const float sHeight);
	~SceneCamera() = default;
	void HandleMouse(const float elapsedSec);
	const FMatrix4 GetViewProjectionMatrix() const;
	const FVector3 GetDirection() const;

private:
	//Mouse settings
	bool m_LeftMouseButton;
	bool m_RightMouseButton;
	
	//Camera settings
	float m_FOV;
	float m_Far;
	float m_Near;
	const float m_ScreenWidth;
	const float m_ScreenHeight;
	const float m_AspectRatio;
	const float m_MouseSensivity;
	const float m_TranslateRate;


	FVector2 m_CameraSpace;
	FMatrix4 m_Matrix;
	FMatrix4 m_ProjectionMatrix;


};

