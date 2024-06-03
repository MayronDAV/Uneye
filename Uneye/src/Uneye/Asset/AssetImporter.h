#pragma once

#include "AssetMetadata.h"

namespace Uneye
{

	class AssetImporter
	{
		public:
			static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};

}
