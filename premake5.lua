include "Dependencies.lua"

workspace "Uneye"
	architecture "x86_64"
	startproject "Uneye-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "Uneye/vendor/GLFW"
	include "Uneye/vendor/Glad"
	include "Uneye/vendor/imgui"
	include "Uneye/vendor/yaml-cpp"
group ""

group "Core"
	include "Uneye"
group ""

group "Tools"
	include "Uneye-Editor"
group ""

group "Misc"
	include "Sandbox"
group ""
