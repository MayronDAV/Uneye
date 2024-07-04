#pragma once

#include "Uneye/Core/Base.h"
#include "Uneye/Scene/Scene.h"
#include "Uneye/Scene/Entity.h"

#include <entt/entt.hpp>

#include <map>


namespace Uneye
{
	// TODO: Make this class static.

	using ScenesMap = std::map<std::filesystem::path, Ref<Scene>>;

	class SceneHierarchyPanel
	{

		public:
			SceneHierarchyPanel() = default;
			SceneHierarchyPanel(const ScenesMap& p_context);
			~SceneHierarchyPanel() = default;

			void SetContext(const ScenesMap& p_context);
			
			void OnImGuiRender();

			Entity GetSelectedEntity() const { return m_SelectionContext; }
			void SetSelectedEntity(Entity entity);

		private:
			template<typename T>
			void DisplayAddComponentEntry(const std::string& entryName);

			void DrawSceneHierarchy(const std::filesystem::path& p_path, const Ref<Scene>& p_scene);
			void DrawEntityNode(Entity entt);
			void DrawComponents(Entity entt);

			template<typename Component, typename Func>
			void DrawComponentUI(Entity entt, const std::string& name, const Func& func, bool settings = false);

		private:
			ScenesMap m_Context = { {} };
			Entity m_SelectionContext = {};
			bool m_OpenedDueToNewEntity = false;
			bool m_OpenedEnttDueToNewEntity = false;
			Ref<Scene> m_SelectedScene = nullptr;
	};
}
