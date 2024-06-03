#include "uypch.h"
#include "Project.h"

#include "ProjectSerializer.h"


namespace Uneye
{
	Ref<Project> Project::New()
	{
		s_ActiveProject = CreateRef<Project>();
		return s_ActiveProject;
	}

	static std::string convertBackslashesToSlashes(const std::string& path) {
		std::string convertedPath = path;
		std::replace(convertedPath.begin(), convertedPath.end(), '\\', '/');
		return convertedPath;
	}

	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();

		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			project->m_ProjectDirectory = path.parent_path();
			s_ActiveProject = project;
			std::shared_ptr<EditorAssetManager> editorAssetManager = std::make_shared<EditorAssetManager>();
			s_ActiveProject->m_AssetManager = editorAssetManager;
			editorAssetManager->DeserializeAssetRegistry();
			return s_ActiveProject;
		}

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		s_ActiveProject->m_ProjectDirectory = path.parent_path();
		return serializer.Serialize(path);
	}
}
