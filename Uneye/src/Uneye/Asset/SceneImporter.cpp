#include "uypch.h"
#include "SceneImporter.h"

#include "Uneye/Project/Project.h"
#include "Uneye/Scene/SceneSerializer.h"

#include <stb_image.h>

namespace Uneye 
{

	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		UNEYE_PROFILE_FUNCTION();

		return LoadScene(Project::GetActiveAssetDirectory() / metadata.FilePath);
	}

	Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		UNEYE_PROFILE_FUNCTION();

		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		if (!serializer.Deserialize(path))
			return nullptr;

		return scene;
	}

	void SceneImporter::SaveScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetActiveAssetDirectory() / path);
	}

}
