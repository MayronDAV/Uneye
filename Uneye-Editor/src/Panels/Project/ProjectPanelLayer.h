#pragma once

#include "Uneye/Core/Layer.h"

#include <string>

namespace Uneye
{
	class ProjectPanelLayer : public Layer
	{
		public:
			ProjectPanelLayer() = default;
			~ProjectPanelLayer();

			virtual void OnAttach() override;

			virtual void OnImGuiRender() override;

		private:
			static bool m_Running;
	};
}
