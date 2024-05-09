#pragma once

#include "Uneye/Core/Base.h"
#include "Uneye/Scene/Scene.h"
#include "Uneye/Scene/Entity.h"

#include <entt/entt.hpp>



namespace Uneye
{
	class SceneHierarchyPanel
	{
		public:
			SceneHierarchyPanel() = default;
			SceneHierarchyPanel(const Ref<Scene>& context);

			void SetContext(const Ref<Scene>& context);
			
			void OnImGuiRender();

		private:
			void DrawEntityNode(Entity entt);
			void DrawComponents(Entity entt);

			template<typename Component, typename Func>
			void DrawComponentUI(Entity entt, const std::string& name, const Func& func);

		private:
			Ref<Scene> m_Context;
			Entity m_SelectionContext;
	};
}