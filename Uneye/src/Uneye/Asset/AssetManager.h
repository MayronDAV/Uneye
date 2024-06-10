#pragma once

#include "AssetManagerBase.h"

#include "Uneye/Project/Project.h"

namespace Uneye
{

	class AssetManager
	{
		public:
			template<typename T>
			static Ref<T> GetAsset(AssetHandle p_handle)
			{
				Ref<Asset> asset = Project::GetActive()->GetAssetManager()->GetAsset(p_handle);
				return std::static_pointer_cast<T>(asset);
			}

			static std::map<AssetHandle, AssetMetadata> GetAssetRegistry()
			{
				return Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
			}


			static void RemoveAsset(AssetHandle p_handle)
			{
				Project::GetActive()->GetEditorAssetManager()->RemoveAsset(p_handle);
			}

			static AssetHandle ImportAsset(const std::filesystem::path& p_filepath)
			{
				return Project::GetActive()->GetEditorAssetManager()->ImportAsset(p_filepath);
			}

			static bool IsAssetHandleValid(AssetHandle p_handle)
			{
				return Project::GetActive()->GetAssetManager()->IsAssetHandleValid(p_handle);
			}

			static bool IsAssetLoaded(AssetHandle p_handle)
			{
				return Project::GetActive()->GetAssetManager()->IsAssetLoaded(p_handle);
			}

			static AssetType GetAssetType(AssetHandle p_handle)
			{
				return Project::GetActive()->GetAssetManager()->GetAssetType(p_handle);
			}
	};
}

