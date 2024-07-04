#pragma once

#include <Uneye/Asset/Asset.h>

#include <glm/glm.hpp>
#include <string>
#include <filesystem>
#include <map>



namespace Uneye
{
	struct Tile
	{
		glm::vec4 Color = { 1, 1, 1, 1 };

		bool IsSubTexture = false;
		glm::vec2 TileSize = { 1, 1 };
		glm::vec2 TileCoord = { 0, 0 };
		glm::vec2 SpriteSize = { 1, 1 };

		Tile() = default;
		Tile(const Tile&) = default;
	};

	struct TileMap
	{
		int Width = 32;
		int Height = 32;
		int TileSize = 16;
		std::map<AssetHandle, Tile> Tiles;

		TileMap() = default;
		TileMap(const TileMap&) = default;
	};
}
