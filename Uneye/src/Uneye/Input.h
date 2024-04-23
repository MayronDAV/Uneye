#pragma once

#include "Uneye/Core.h"

namespace Uneye
{
	class UNEYE_API Input
	{
		public:
			// Key stuff
			// ----------------------
			inline static bool isKeyPressed(int keycode) {
				return s_Instance->isKeyPressedImpl(keycode); 
			};

			// Mouse stuff
			// ----------------------
			inline static bool isMouseButtonPressed(int button) {
				return s_Instance->isMouseButtonPressedImpl(button);
			};
			inline static std::pair<float, float> GetMousePos() {
				return s_Instance->GetMousePosImpl();
			};
			inline static float GetMouseX() {
				return s_Instance->GetMouseXImpl();
			};
			inline static float GetMouseY() {
				return s_Instance->GetMouseYImpl();
			};



		protected:
			virtual bool isKeyPressedImpl(int keycode) = 0;
			virtual bool isMouseButtonPressedImpl(int button) = 0;
			virtual std::pair<float, float> GetMousePosImpl() = 0;
			virtual float GetMouseXImpl() = 0;
			virtual float GetMouseYImpl() = 0;

		private:
			static Input* s_Instance;
	};


};