
-- Uneye Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb"] = "%{wks.location}/Uneye/vendor/stb/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Uneye/vendor/yaml-cpp/include"
IncludeDir["GLFW"] = "%{wks.location}/Uneye/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Uneye/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Uneye/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Uneye/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/Uneye/vendor/entt/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Uneye/vendor/ImGuizmo"
IncludeDir["shaderc"] = "%{wks.location}/Uneye/vendor/shaderc/libshaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Uneye/vendor/SPIRV-Cross/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"