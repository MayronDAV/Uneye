workspace "Uneye"
	architecture "x64"
	startproject "Uneye-Editor"

	defines
	{
		"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
		"_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING"
	}

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Uneye/vendor/GLFW/include"
IncludeDir["Glad"] = "Uneye/vendor/Glad/include"
IncludeDir["ImGui"] = "Uneye/vendor/imgui"
IncludeDir["glm"] = "Uneye/vendor/glm"
IncludeDir["stb"] = "Uneye/vendor/stb/include"
IncludeDir["assets"] = "Uneye/assets"

include "Uneye/vendor/GLFW"
include "Uneye/vendor/Glad"
include "Uneye/vendor/imgui"

project "Uneye"
	location "Uneye"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "uypch.h"
	pchsource "Uneye/src/uypch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb/src/**.c",
		"%{prj.name}/vendor/stb/include/**.h",
		"%{prj.name}/vendor/spdlog/include/**.h",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/assets/*"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.assets}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"UNEYE_PLATFORM_WINDOWS",
			"UNEYE_BUILD_DLL",
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

project "Uneye-Editor"
	location "Uneye-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/assets",
		"Uneye/vendor/spdlog/include",
		"Uneye/src",
		"Uneye/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Uneye"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"UNEYE_PLATFORM_WINDOWS"
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

project "Sandbox"
		location "Sandbox"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"
	
		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp"
		}
	
		includedirs
		{
			"%{prj.name}/src",
			"%{prj.name}/assets",
			"Uneye/vendor/spdlog/include",
			"Uneye/src",
			"Uneye/vendor",
			"%{IncludeDir.glm}"
		}
	
		links
		{
			"Uneye"
		}
	
		filter "system:windows"
			systemversion "latest"
	
			defines
			{
				"UNEYE_PLATFORM_WINDOWS"
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
