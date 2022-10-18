//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include "SDL_events.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"
#include "Vector3.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow)),
	m_AreShadowsEnabled{true}
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
	m_WidthFloat = static_cast<float>(m_Width);
	m_HeightFloat = static_cast<float>(m_HeightFloat);

}

void Renderer::Render(Scene* pScene) 
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();
	Vector3 rayDirection(0,0,0);
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			// Raster space to camera space
			const float	px_c{ float(px) + 0.5f }, 
					py_c{ py + 0.5f };
				  
			const float	c_x{ ((2 * px_c) - m_WidthFloat) * camera.FOV / m_WidthFloat },
						c_y{ 1 - ((2 * py_c) * camera.FOV / m_WidthFloat) };

			// Make appropriate ray & normalize
			rayDirection.x = c_x;
			rayDirection.y = c_y;
			rayDirection.z = 1.f;
			rayDirection = rayDirection.Normalized();

			// Camera space to world space
			const Matrix cameraToWorld = camera.CalculateCameraToWorld();
			rayDirection = cameraToWorld.TransformVector(rayDirection);

			//Create & fill in hit record with the current view ray
			ColorRGB finalColor{};
			Ray viewRay{ camera.origin , rayDirection };
			HitRecord closestHit{};
			pScene->GetClosestHit(viewRay, closestHit);


			//If we hit something, give it it's appropriate color
				//Loop over the lights & apply the rendering equation
				for (size_t lightIdx = 0; lightIdx < lights.size(); ++lightIdx)
				{
					if (closestHit.didHit)
					{
						Vector3 directionToLight = LightUtils::GetDirectionToLight(lights[lightIdx], closestHit.origin + closestHit.normal * 0.01f);
						const float LambertCosine{ LightUtils::GetLambertCosine(closestHit.normal, directionToLight.Normalized()) };
						if (LambertCosine != 0.f)
						{
							switch (m_CurrentLightingMode)
							{
							case LightingMode::ObservedArea:
								{
									finalColor += ColorRGB{ LambertCosine ,LambertCosine ,LambertCosine };
									break;
								}
							case LightingMode::Radiance:
								{
									finalColor += LightUtils::GetRadiance(lights[lightIdx], closestHit.origin);
									break;
								}
							case LightingMode::BRDF:
								{
									finalColor += materials[closestHit.materialIndex]->Shade(closestHit, directionToLight.Normalized(), -viewRay.direction);
									break;
								}
							case LightingMode::Combined:
								{
									finalColor += LightUtils::GetRadiance(lights[lightIdx], closestHit.origin) * materials[closestHit.materialIndex]->Shade(closestHit, directionToLight.Normalized(), -viewRay.direction) * LambertCosine;
									
									break;
								}
							}
							//Apply shadows
							if (m_AreShadowsEnabled)
							{
								Ray lightRay{ closestHit.origin + closestHit.normal * 0.01f, directionToLight.Normalized(), 0.0001f, directionToLight.Magnitude() };
								if (pScene->DoesHit(lightRay))
								{
									finalColor *= 0.5f;
								}
							}
						}
					}
				}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void Renderer::ProcessKeyUpEvent(const SDL_Event& e)
{
	switch (e.key.keysym.scancode)
	{
	case SDL_SCANCODE_F2:
		ToggleShadows();
		break;
	case SDL_SCANCODE_F3:
		TogglelightingMode();
		break;
	default:
		break;
	}

	PrintCurrentSceneState();

}


void dae::Renderer::ToggleShadows()
{
	m_AreShadowsEnabled = !m_AreShadowsEnabled;
}

void dae::Renderer::TogglelightingMode()
{
	m_CurrentLightingMode = static_cast<LightingMode>((static_cast<int>(m_CurrentLightingMode) + 1) % 4);
}

void dae::Renderer::PrintCurrentSceneState() const 
{
	if (m_AreShadowsEnabled)
	{
		std::cout << "Shadows are enabled" << "\n";
	}
	else
	{
		std::cout << "Shadows are disabled" << "\n";
	}
	switch (m_CurrentLightingMode)
	{
	case LightingMode::ObservedArea:
		std::cout << "Lighting mode is set to ObservedArea" << "\n";
		break;
	case LightingMode::Radiance:
		std::cout << "Lighting mode is set to Radiance" << "\n";
		break;
	case LightingMode::BRDF:
		std::cout << "Lighting mode is set to BRDF" << "\n";
		break;
	case LightingMode::Combined:
		std::cout << "Lighting mode is set to Combined" << "\n";
		break;
	default:
		break;
	}
}

