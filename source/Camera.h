#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}

		const float movementSpeed{ 20.f };
		const float rotationSpeed{ 2.f };
		Vector3 origin{};
		float fovAngle{90.f};
		float FOV = tanf((fovAngle * TO_RADIANS) / 2.f);

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			//todo W2 DONE
			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			Matrix ONB{
			Vector4{ right, 0.f},
			Vector4{ up, 0.f},
			Vector4{forward, 0.f},
			Vector4{ origin ,1.f}
			};

			
			return ONB;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += movementSpeed * deltaTime * forward;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin += movementSpeed * deltaTime * -right;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin += movementSpeed * deltaTime * -forward;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += movementSpeed * deltaTime * right;
			}

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if (mouseState & SDL_BUTTON(1))
			{

				if (mouseState & SDL_BUTTON(3))
				{
					if (mouseX > 0)
					{
						origin += movementSpeed * deltaTime * up;
					}
					else if (mouseX < 0)
					{
						origin += movementSpeed * deltaTime * -up;
					}
				}
				else
				{
					if (mouseY > 0)
					{
						origin += movementSpeed * deltaTime * -forward;
					}
					else if (mouseY < 0)
					{
						origin += movementSpeed * deltaTime * forward;
					}
					if (mouseX > 0)
					{
						totalYaw += rotationSpeed * deltaTime;
					}
					else if (mouseX < 0)
					{
						totalYaw -= rotationSpeed * deltaTime;
					}
				}


			}
			else if (mouseState & SDL_BUTTON(3))
			{

				if (mouseX < 0)
				{
					totalYaw -= rotationSpeed * deltaTime;
				}
				else if (mouseX > 0)
				{
					totalYaw += rotationSpeed * deltaTime;
				}
				if (mouseY > 0)
				{
					totalPitch -= rotationSpeed * deltaTime;
				}
				else if (mouseY < 0)
				{
					totalPitch += rotationSpeed * deltaTime;
				}
			}

			Matrix finalRotation{ Matrix::CreateRotation({ totalPitch, totalYaw, 0 }) };
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();
			//todo: W2 DONE
		}
	};
}
