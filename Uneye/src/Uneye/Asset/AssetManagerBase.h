#pragma once

#include "Asset.h"
#include "Uneye/Core/Base.h"

#include <map>

namespace Uneye
{

	using AssetMap = std::map<AssetHandle, Ref<Asset>>;

	class AssetManagerBase
	{
		public:
			virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;

			virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
			virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
			virtual AssetType GetAssetType(AssetHandle handle) const = 0;
	};
}
