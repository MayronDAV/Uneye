project "Sandbox"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"
	
		targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
		objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	
		files
		{
			"src/**.h",
			"src/**.cpp"
		}
	
		includedirs
		{
			"%{wks.location}/Uneye/vendor/spdlog/include",
			"%{wks.location}/Uneye/src",
			"%{wks.location}/Uneye/vendor",
			"%{IncludeDir.glm}",
			"%{IncludeDir.entt}"
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
