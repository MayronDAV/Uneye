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
	static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
		{ ".uyscene", AssetType::Scene },
		{ ".png", AssetType::Texture2D },
		{ ".jpg", AssetType::Texture2D },
		{ ".jpeg", AssetType::Texture2D }
	};

	static AssetType GetAssetTypeFromFileExtension(const std::filesystem::path& extension)
	{
		if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
		{
			UNEYE_CORE_WARN("Could not find AssetType for {}", extension);
			return AssetType::None;
		}

		return s_AssetExtensionMap.at(extension);
	}

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

	FilePathAssetRegistry EditorAssetManager::s_FilePathAssetRegistry;

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


	void EditorAssetManager::RemoveAsset(AssetHandle p_handle)
	{
		auto it = m_AssetRegistry.find(p_handle);
		if (it != m_AssetRegistry.end())
		{
			auto& metadata = m_AssetRegistry.at(p_handle);
			bool isFPAsset = s_FilePathAssetRegistry.find(metadata.FilePath) != s_FilePathAssetRegistry.end();
			if (isFPAsset) s_FilePathAssetRegistry.erase(metadata.FilePath);

			bool isLoaded = m_LoadedAssets.find(p_handle) != m_LoadedAssets.end();
			if (isLoaded) m_LoadedAssets.erase(p_handle);

			m_AssetRegistry.erase(p_handle);

			SerializeAssetRegistry();
		}
		else
		{
			UNEYE_CORE_ERROR("This asset not exists");
		}
	}

	AssetHandle EditorAssetManager::ImportAsset(const std::filesystem::path& p_filepath)
	{
		AssetHandle handle; // generate new handle

		auto it = s_FilePathAssetRegistry.find(p_filepath);
		if (it == s_FilePathAssetRegistry.end())
		{
			AssetMetadata metadata;
			metadata.FilePath = p_filepath;
			metadata.Type = GetAssetTypeFromFileExtension(p_filepath.extension());
			UNEYE_CORE_ASSERT(metadata.Type == AssetType::None);
			Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
			if (asset)
			{
				asset->Handle = handle;
				m_LoadedAssets[handle] = asset;
				m_AssetRegistry[handle] = metadata;
				s_FilePathAssetRegistry[metadata.FilePath] = std::make_pair(handle, metadata);
				SerializeAssetRegistry();
			}
		}
		else
		{
			handle = it->second.first;
		}

		return handle;
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
			m_LoadedAssets[p_handle] = asset;
		}
		// 3. return asset
		return asset;
	}

	void EditorAssetManager::SerializeAssetRegistry()
	{
		UNEYE_PROFILE_FUNCTION();

		auto path = Project::GetActiveAssetRegistryPath();
		
		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "AssetRegistry" << YAML::Value;

			out << YAML::BeginSeq;
			UNEYE_CORE_INFO("Serializing AssetRegistry");
			for (const auto& [handle, metadata] : m_AssetRegistry)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << (uint64_t)handle;
				std::string filepathStr = std::filesystem::relative(metadata.FilePath, Project::GetActiveProjectDirectory()).generic_string();
				out << YAML::Key << "FilePath" << YAML::Value << filepathStr;
				out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type).data();
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap; // Root
		}

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
			metadata.FilePath = Project::GetActiveProjectDirectory() / node["FilePath"].as<std::string>();
			metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());

			s_FilePathAssetRegistry[metadata.FilePath] = std::make_pair(handle, metadata);
		}

		return true;
	}
}
