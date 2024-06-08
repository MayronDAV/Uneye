#pragma once

#include "Uneye/Core/Layer.h"

#include "Uneye/Events/ApplicationEvent.h"
#include "Uneye/Events/KeyEvent.h"
#include "Uneye/Events/MouseEvent.h"



namespace Uneye {

	class ImGuiLayer : public Layer
	{
		public:
			ImGuiLayer();
			~ImGuiLayer();

			virtual void OnAttach() override;
			virtual void OnDetach() override;
			virtual void OnEvent(Event& event) override;

			void BlockEvents(bool block) { m_BlockEvents = block; }

			void Begin();
			void End();

			uint32_t GetActiveWidgetID() const;

		private:
			bool m_BlockEvents = true;
	};

}
