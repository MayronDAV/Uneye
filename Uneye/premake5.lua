project "Uneye"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "uypch.h"
	pchsource "src/uypch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/stb/src/**.c",
		"vendor/stb/src/**.cpp",
		"vendor/stb/include/**.h",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"UNEYE_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "UNEYE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "UNEYE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "UNEYE_DIST"
		runtime "Release"
		optimize "on"