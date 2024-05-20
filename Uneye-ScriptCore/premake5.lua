project "Uneye-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{wks.location}/Uneye-Editor/Resources/Scripts")
	objdir ("%{wks.location}/Uneye-Editor/Resources/Scripts/Intermediates")

	files
	{
		"Source/**.cs",
		"Properties/**.cs"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"