project "Uneye-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.rc"
	}

	includedirs
	{
		"src",
		"%{wks.location}/Uneye/vendor/spdlog/include",
		"%{wks.location}/Uneye/src",
		"%{wks.location}/Uneye/vendor",
		"%{IncludeDir.entt}",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.stb}"
	}

	links "Uneye"

	filter "system:windows"
		systemversion "latest"
		defines "UNEYE_PLATFORM_WINDOWS"

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
