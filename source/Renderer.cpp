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
			float	px_c{ px + 0.5f }, 
					py_c{ py + 0.5f };
				  
			float	c_x{ ((2 * px_c) - m_Width) / m_Height },
					c_y{ 1 - (2 * py_c) / m_Height };

			// Make appropriate ray
			rayDirection.x = c_x;
			rayDirection.y = c_y;
			rayDirection.z = 1.f;

			rayDirection = rayDirection.Normalized();

			Ray hitRay{ {0,0,0} , rayDirection };
			HitRecord closestHit{};
			ColorRGB finalColor{};
			Sphere testSphere{ {0.f,0.f,100.f}, 50.f,0 };

			GeometryUtils::HitTest_Sphere(testSphere, hitRay, closestHit);

			//Give pixel appropriate color
			if (closestHit.didHit)
			{
				const float scaled_t = (closestHit.t - 50.f) / 40.f;
				//finalColor = materials[closestHit.materialIndex]->Shade();
				finalColor = { scaled_t,scaled_t,scaled_t };
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

