#pragma once

#include "Uneye/Core/Base.h"
#include "Uneye/Scene/Scene.h"

#include <string>



namespace Uneye
{
	class SceneSerializer
	{
		public:
			SceneSerializer(const Ref<Scene>& scene);

			void Serialize(const std::filesystem::path& filepath);
			void SerializeRuntime(const std::filesystem::path& filepath);

			bool Deserialize(const std::filesystem::path& filepath);
			bool DeserializeRuntime(const std::filesystem::path& filepath);

		private:
			Ref<Scene> m_Scene;
	};
}
