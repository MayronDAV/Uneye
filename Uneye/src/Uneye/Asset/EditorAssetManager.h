#pragma once

#include "AssetManagerBase.h"
#include "AssetMetadata.h"

#include <map>

namespace Uneye
{

	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

	// Editor Only
	using FilePathAssetRegistry = std::unordered_map<std::filesystem::path, std::pair<AssetHandle, AssetMetadata>>;

	class EditorAssetManager : public AssetManagerBase
	{
		public:
			virtual Ref<Asset> GetAsset(AssetHandle p_handle) override;

			virtual bool IsAssetHandleValid(AssetHandle p_handle) const override;
			virtual bool IsAssetLoaded(AssetHandle p_handle) const override;
			virtual AssetType GetAssetType(AssetHandle p_handle) const override;

			void RemoveAsset(AssetHandle p_handle);
			AssetHandle ImportAsset(const std::filesystem::path& p_filepath);

			const AssetMetadata& GetMetadata(AssetHandle p_handle) const;
			const std::filesystem::path& GetFilePath(AssetHandle p_handle) const;

			const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }

			void SerializeAssetRegistry();
			bool DeserializeAssetRegistry();

		private:
			AssetRegistry m_AssetRegistry;
			AssetMap m_LoadedAssets;

			// Editor Only
			static FilePathAssetRegistry s_FilePathAssetRegistry;

			// TODO: memory-only assets
	};
}

