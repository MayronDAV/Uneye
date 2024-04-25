#pragma once

#include "Uneye/Layer.h"

#include "Uneye/Events/ApplicationEvent.h"
#include "Uneye/Events/KeyEvent.h"
#include "Uneye/Events/MouseEvent.h"

namespace Uneye {

	enum ImGuiThemes
	{
		PurpleThemeDefault,
		DeepDarkThemeDefault,
		DarkThemeDefault,
		LightThemeDefault
	};

	class UNEYE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void Begin();
		void End();

		void SetDeepDarkThemeColors();
		void SetPurpleThemeColors();
		void SetDarkThemeColors();
		void SetLightThemeColors();


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