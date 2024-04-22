#pragma once

#include <Uneye/Layer.h>

#include "Uneye/Core.h"

#include "Uneye/Events/MouseEvent.h"
#include "Uneye/Events/KeyEvent.h"
#include "Uneye/Events/ApplicationEvent.h"


namespace Uneye
{
	enum ImGuiThemes
	{
		PurpleThemeDefault,
		DeepDarkThemeDefault,
		DarkThemeDefault,
		LightThemeDefault
	};


	class UNEYE_API ImGuiLayer : public Uneye::Layer
	{
		public:
			ImGuiLayer();
			~ImGuiLayer() = default;

			virtual void OnAttach();
			virtual void OnDetach();
			virtual void OnEvent(Event& event);

			void Begin();
			void End();

			void BlockEvents(bool block) { m_BlockEvents = block; }

			void SetDeepDarkThemeColors();
			void SetPurpleThemeColors();
			void SetDarkThemeColors();
			void SetLightThemeColors();

			uint32_t GetActiveWidgetID() const;

		private:
			float m_Time = 0.0f;

			bool m_BlockEvents = true;
			ImGuiThemes m_Theme = PurpleThemeDefault;
			std::unordered_map<int, std::function<void()>> m_AllThemes = {
				{PurpleThemeDefault, [=]() { SetPurpleThemeColors(); }},
				{DeepDarkThemeDefault, [=]() { SetDarkThemeColors(); }},
				{DarkThemeDefault, [=]() { SetDarkThemeColors(); }},
				{LightThemeDefault, [=]() { SetLightThemeColors(); }}
			};
	};

}
