#include "uypch.h"
#include "ProjectSerializer.h"

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
}


namespace Uneye
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const std::filesystem::path& path)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << path.string() << YAML::EndSeq;
		return out;
	}

	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		UNEYE_PROFILE_FUNCTION();

		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;
		out << YAML::BeginMap; // Root
		{
			out << YAML::Key << "Project" << YAML::Value;

			out << YAML::BeginMap; // Project
			{
				out << YAML::Key << "Name" << YAML::Value << config.Name;
				out << YAML::Key << "StartScene" << YAML::Value << config.StartScene;
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
				out << YAML::Key << "CacheDirectory" << YAML::Value << config.CacheDirectory.string();
				out << YAML::Key << "AssetRegistryPath" << YAML::Value << config.AssetRegistryPath.string();
				out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();
			}
			out << YAML::EndMap; // Project
		}
		out << YAML::EndMap; // Root

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		UNEYE_PROFILE_FUNCTION();

		auto& config = m_Project->GetConfig();


		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			UNEYE_CORE_ERROR("Failed to load .uyproj file '{0}'\n     {1}", filepath, e.what());
			return false;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.Name = projectNode["Name"].as<std::string>();
		config.StartScene = projectNode["StartScene"].as<uint64_t>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		config.CacheDirectory = projectNode["CacheDirectory"].as<std::string>();
		config.AssetRegistryPath = projectNode["AssetRegistryPath"].as<std::string>();
		config.ScriptModulePath = projectNode["ScriptModulePath"].as<std::string>();

		return true;
	}
}
