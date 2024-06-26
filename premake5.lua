include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"


workspace "Uneye"
	architecture "x64"
	startproject "Uneye-Editor"	
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
	solution_items
	{
		".editorconfig"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}
	
	defines
	{
		"_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
		"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
	}
	
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
	
group "Dependencies"
	include "vendor/premake"
	include "Uneye/vendor/GLFW"
	include "Uneye/vendor/Glad"
	include "Uneye/vendor/msdf-atlas-gen"
	include "Uneye/vendor/imgui"
	include "Uneye/vendor/Box2D"
	include "Uneye/vendor/yaml-cpp"
	include "Uneye/vendor/zlib"
	include "Uneye/vendor/stb"
group ""

group "Core"
	include "Uneye"
	include "Uneye-ScriptCore"
group ""
	
group "Tools"
	include "Uneye-Editor"
group ""
