#pragma once

#include "Uneye/Asset/Asset.h"

#include <string>
#include <glm/glm.hpp>


namespace Uneye
{

	class AssetImporterPanel
	{
		public:
			AssetImporterPanel();
			~AssetImporterPanel() = default;

			void Close();
			void Open();

			void SetOpen() { m_IsOpen = true; m_WasOpen = true; }
			operator const bool&() { return m_IsOpen; }

		private:
			struct AssetImporterOptions
			{
				std::string_view FilePath = "";
				AssetType Type = AssetType::None;
				glm::vec2 TileSize{ 1, 1 };
				glm::vec2 TileCoord{ 0, 0 };
				glm::vec2 SpriteSize{ 1, 1 };
			};

			bool m_IsOpen = false;
			bool m_WasOpen = m_IsOpen;
			AssetImporterOptions m_Options;

			static AssetImporterPanel* s_Instance;
	};
}
