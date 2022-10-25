#pragma once

#include <cstdint>
#include <vector>


struct SDL_Window;
struct SDL_Surface;
struct ColorRGB;

namespace dae
{
	class Scene;
	class Camera;
	class Material;
	struct Light;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) ;
		void RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Camera& camera, const std::vector<Light>& lights, const std::vector<Material*>& materials) const;
		bool SaveBufferToImage() const;
		void ProcessKeyUpEvent(const SDL_Event& e);

		enum class LightingMode
		{
			ObservedArea = 0, //Lambert Cosine Law
			Radiance = 1, //Incident Radiance
			BRDF = 2, //Scattering of the light
			Combined = 3 // ObservedArea*Radiance*BRDF
		};
		LightingMode m_CurrentLightingMode{ LightingMode::Combined };
	private:


		void ToggleShadows();
		void TogglelightingMode();
		void PrintCurrentSceneState() const;
		SDL_Window* m_pWindow{};
		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};
		float m_WidthFloat{};
		float m_HeightFloat{};
		bool m_AreShadowsEnabled{};
	};
}
