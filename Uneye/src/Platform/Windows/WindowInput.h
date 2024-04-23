#pragma once

#include "Uneye/Input.h"



namespace Uneye
{
	class WindowInput : public Uneye::Input
	{
		protected:
			virtual bool isKeyPressedImpl(int keycode) override;
			virtual bool isMouseButtonPressedImpl(int button) override;
			virtual std::pair<float, float> GetMousePosImpl() override;
			virtual float GetMouseXImpl() override;
			virtual float GetMouseYImpl() override;

	};
};

