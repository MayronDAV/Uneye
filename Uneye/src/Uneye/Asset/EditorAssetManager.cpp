#include "uypch.h"
#include "EditorAssetManager.h"

#include "AssetImporter.h"

#include "Uneye/Project/Project.h"

#include <fstream>
#include <yaml-cpp/yaml.h>


namespace YAML {

	template<>
	struct convert<Uneye::UUID>
	{
		static Node encode(const Uneye::UUID& p_uuid)
		{
			Node node;
			node.push_back((uint64_t)p_uuid);
			return node;
		}

		static bool decode(const Node& node, Uneye::UUID& p_uuid)
		{
			p_uuid = node.as<uint64_t>();
			return true;
		}
	};

	template<>
	struct convert<std::string_view>
	{
		static Node encode(const std::string_view& p_v)
		{
			Node node;
			node.push_back(std::string(p_v.data(), p_v.size()));
			return node;
		}

		static bool decode(const Node& p_node, std::string_view& p_v)
		{
			p_v = p_node.as<std::string_view>();
			return true;
		}
	};
}



namespace Uneye
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const std::string_view& p_v)
	{
		out << std::string(p_v.data(), p_v.size());
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const AssetHandle& p_handle)
	{
		out << (uint64_t)p_handle;
		return out;
	}

	bool EditorAssetManager::IsAssetHandleValid(AssetHandle p_handle) const
	{
		return p_handle != 0 && m_AssetRegistry.find(p_handle) != m_AssetRegistry.end();
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle p_handle) const
	{
		return m_LoadedAssets.find(p_handle) != m_LoadedAssets.end();
	}

	AssetType EditorAssetManager::GetAssetType(AssetHandle p_handle) const
	{
		if (!IsAssetHandleValid(p_handle))
			return AssetType::None;

		return m_AssetRegistry.at(p_handle).Type;
	}

	void EditorAssetManager::ImportAsset(const std::filesystem::path& p_filepath)
	{
		AssetHandle handle; // generate new handle
		AssetMetadata metadata;
		metadata.FilePath = p_filepath;
		metadata.Type = AssetType::Texture2D; // TODO(Yan): grab this from extension and try to load
		Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
		if (asset)
		{
			asset->Handle = handle;
			m_LoadedAssets[handle] = asset;
			m_AssetRegistry[handle] = metadata;
			SerializeAssetRegistry();
		}
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(AssetHandle p_handle) const
	{
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetRegistry.find(p_handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}

	const std::filesystem::path& EditorAssetManager::GetFilePath(AssetHandle p_handle) const
	{
		return GetMetadata(p_handle).FilePath;
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle p_handle)
	{
		// 1. check if handle is valid
		if (!IsAssetHandleValid(p_handle))
			return nullptr;

		// 2. check if asset needs load (and if so, load)
		Ref<Asset> asset;
		if (IsAssetLoaded(p_handle))
		{
			asset = m_LoadedAssets.at(p_handle);
		}
		else
		{
			// load asset
			const AssetMetadata& metadata = GetMetadata(p_handle);
			asset = AssetImporter::ImportAsset(p_handle, metadata);
			if (!asset)
			{
				// import failed
				UNEYE_CORE_ERROR("EditorAssetManager::GetAsset - asset import failed!");
			}
		}
		// 3. return asset
		return asset;
	}

	void EditorAssetManager::SerializeAssetRegistry()
	{
		UNEYE_PROFILE_FUNCTION();

		auto path = Project::GetActiveAssetRegistryPath();
		YAML::Node rootNode;

		try 
		{
			rootNode = YAML::LoadFile(path.string());
		}
		catch (...) 
		{
			rootNode = YAML::Node(YAML::NodeType::Map);
		}

		if (!rootNode["AssetRegistry"] || !rootNode["AssetRegistry"].IsSequence()) 
			rootNode["AssetRegistry"] = YAML::Node(YAML::NodeType::Sequence);

		YAML::Node assetRegistryNode = rootNode["AssetRegistry"];

		for (const auto& [handle, metadata] : m_AssetRegistry)
		{
			YAML::Node assetNode;
			assetNode["Handle"] = (uint64_t)handle;
			std::string filepathStr = metadata.FilePath.generic_string();
			assetNode["FilePath"] = filepathStr;
			assetNode["Type"] = AssetTypeToString(metadata.Type).data();

			assetRegistryNode.push_back(assetNode);
		}

		YAML::Emitter out;
		out << rootNode;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		UNEYE_PROFILE_FUNCTION();

		auto path = Project::GetActiveAssetRegistryPath();
		YAML::Node data;

		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (...) 
		{
			UNEYE_CORE_ERROR(" Failed to load file {0}. Does this file exist?", path);
			return false;
		}

		auto rootNode = data["AssetRegistry"];
		if (!rootNode)
			return false;

		for (const auto& node : rootNode)
		{
			AssetHandle handle = node["Handle"].as<uint64_t>();
			auto& metadata = m_AssetRegistry[handle];
			metadata.FilePath = node["FilePath"].as<std::string>();
			metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());

			UNEYE_CORE_INFO(" - Handle: {}", handle);
			UNEYE_CORE_INFO("	Filepath: {}", metadata.FilePath);
			UNEYE_CORE_INFO("	Type: {}", AssetTypeToString(metadata.Type));
		}

		return true;
	}
}
