
-- Uneye Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb"] = "../Uneye/vendor/stb/include"
IncludeDir["yaml_cpp"] = "../Uneye/vendor/yaml-cpp/include"
IncludeDir["GLFW"] = "../Uneye/vendor/GLFW/include"
IncludeDir["Glad"] = "../Uneye/vendor/Glad/include"
IncludeDir["ImGui"] = "../Uneye/vendor/imgui"
IncludeDir["glm"] = "../Uneye/vendor/glm"
IncludeDir["Box2D"] = "../Uneye/vendor/Box2D/include"
IncludeDir["filewatch"] = "../Uneye/vendor/filewatch"
IncludeDir["entt"] = "../Uneye/vendor/entt/include"
IncludeDir["mono"] = "../Uneye/vendor/mono/include"
IncludeDir["ImGuizmo"] = "../Uneye/vendor/ImGuizmo"
IncludeDir["shaderc"] = "../Uneye/vendor/shaderc/libshaderc/include"
IncludeDir["SPIRV_Cross"] = "../Uneye/vendor/SPIRV-Cross/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["msdf_atlas_gen"] = "../Uneye/vendor/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["msdfgen"] = "../Uneye/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["zlib"] = "../Uneye/vendor/zlib"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["mono"] = "../Uneye/vendor/mono/lib/%{cfg.buildcfg}"

Library = {}

Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"
