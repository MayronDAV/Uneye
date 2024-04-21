workspace "Uneye"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Uneye/thirdyparty/GLFW/include"

include "Uneye/thirdyparty/GLFW"

project "Uneye"
	location "Uneye"
	kind "SharedLib"
	language "C++"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "Uneyepch.h"
	pchsource "Uneye/src/Uneyepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/thirdyparty/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"UNEYE_PLATFORM_WINDOWS",
			"UNEYE_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		runtime "Debug"
		defines "UNEYE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		defines "UNEYE_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		defines "UNEYE_DIST"
		optimize "On"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Uneye/thirdyparty/spdlog/include",
		"Uneye/src"
	}

	links
	{
		"Uneye"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"UNEYE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		defines "UNEYE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		defines "UNEYE_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		defines "UNEYE_DIST"
		optimize "On"