#pragma once

#include <cstdint>

struct SDL_Window;
struct SDL_Surface;
struct ColorRGB;

namespace dae
{
	class Scene;

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
		bool SaveBufferToImage() const;
		void ProcessKeyUpEvent(const SDL_Event& e);

		enum class LightingMode
		{
			ObservedArea = 0, //Lambert Cosine Law
			Radiance = 1, //Incident Radiance
			BRDF = 2, //Scattering of the light
			Combined = 3 // ObservedArea*Radiance*BRDF
		};
		LightingMode m_CurrentLightingMode{ LightingMode::BRDF };
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
