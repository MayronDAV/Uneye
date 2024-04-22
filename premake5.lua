startproject "Sandbox"

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
IncludeDir["Glad"] = "Uneye/thirdyparty/Glad/include"
IncludeDir["ImGui"] = "Uneye/thirdyparty/imgui"

include "Uneye/thirdyparty/GLFW"
include "Uneye/thirdyparty/Glad"
include "Uneye/thirdyparty/imgui"

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
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
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
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		defines "UNEYE_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		defines "UNEYE_DIST"
		buildoptions "/MD"
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
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		defines "UNEYE_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		defines "UNEYE_DIST"
		buildoptions "/MD"
		optimize "On"