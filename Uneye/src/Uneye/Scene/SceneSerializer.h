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

			void Serialize(const std::string& filepath);
			void SerializeRuntime(const std::string& filepath);

			bool Deserialize(const std::string& filepath);
			bool DeserializeRuntime(const std::string& filepath);

		private:
			Ref<Scene> m_Scene;
	};
}
