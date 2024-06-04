#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "Uneye/Scene/Scene.h"



namespace Uneye
{

	class SceneImporter
	{
		public:
			// AssetMetadata filepath is relative to project asset directory
			static Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);

			// Load from filepath
			static Ref<Scene> LoadScene(const std::filesystem::path& path);

			static void SaveScene(Ref<Scene> scene, const std::filesystem::path& path);
	};



}
