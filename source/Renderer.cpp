//External includes
#include "SDL.h"
#include "SDL_surface.h"

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
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
	m_WidthFloat = static_cast<float>(m_Width);
	m_HeightFloat = static_cast<float>(m_HeightFloat);

}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();
	Vector3 rayDirection(0,0,0);

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			// Raster space to camera space
			const float	px_c{ px + 0.5f }, 
					py_c{ py + 0.5f };
				  
			const float	c_x{ ((2 * px_c) - m_WidthFloat) * camera.FOV / m_WidthFloat },
						c_y{ 1 - ((2 * py_c) * camera.FOV / m_WidthFloat) };

			// Make appropriate ray
			rayDirection.x = c_x;
			rayDirection.y = c_y;
			rayDirection.z = 1.f;

			rayDirection = rayDirection.Normalized();

			const Matrix cameraToWorld = camera.CalculateCameraToWorld();
			rayDirection = cameraToWorld.TransformVector(rayDirection);

			ColorRGB finalColor{};
			Ray viewRay{ camera.origin , rayDirection };
			HitRecord closestHit{};
			//Plane testPlane{ {0.f,-50.f,0.f },{0.f,1.f,0.f }, 0 };
			//GeometryUtils::HitTest_Plane(testPlane, hitRay, closestHit);
			pScene->GetClosestHit(viewRay, closestHit);


			//Give pixel appropriate color
			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();
				for (size_t lightIdx = 0; lightIdx < lights.size(); lightIdx++)
				{
					Vector3 directionToLight = LightUtils::GetDirectionToLight(lights[lightIdx], closestHit.origin + closestHit.normal * 0.01f);
					Ray lightRay{ closestHit.origin + closestHit.normal * 0.01f, directionToLight.Normalized(), 0.0001f, directionToLight.Magnitude() };
					if (pScene->DoesHit(lightRay))
					{
						finalColor * 0.5f;
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

