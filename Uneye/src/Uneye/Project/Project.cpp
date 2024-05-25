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

	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		auto project = CreateRef<Project>();
		ProjectSerializer serializer(project);

		if (!serializer.Deserialize(path))
			return nullptr;

		s_ActiveProject = project;
		s_ActiveProject->m_ProjectDirectory = path.parent_path();
		return s_ActiveProject;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		s_ActiveProject->m_ProjectDirectory = path.parent_path();
		return serializer.Serialize(path);
	}
}
