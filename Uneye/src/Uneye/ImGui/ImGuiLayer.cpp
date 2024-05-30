#include "uypch.h"
#include "ImGuiLayer.h"

#include "Uneye/Core/Application.h"

#include "Uneye/Core/FontManager.h"
#include "Uneye/Core/ThemeManager.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <ImGuizmo.h>

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>





namespace Uneye {

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		UNEYE_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // Habilita escala de DPI em viewports
		io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts; // Habilita escala de DPI em fonts
		io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // Habilita escala de DPI em viewports

		for (const auto& entry : std::filesystem::recursive_directory_iterator("Themes")) {
			if (entry.is_regular_file() && entry.path().extension() == ".uythm")
				ThemeManager::LoadTheme(entry.path().string());
		}

		auto themeFunc = ThemeManager::GetTheme(Application::Get().GetTheme());
		if (themeFunc != NULL) themeFunc();

		float fontSize = 18.0f;
		auto font0 = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto_Slab/static/RobotoSlab-Regular.ttf", fontSize);
		auto font1 = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto_Slab/static/RobotoSlab-Bold.ttf", fontSize);
		auto font2 = io.Fonts->AddFontFromFileTTF("assets/fonts/arrow/Arrows-Regular.ttf", fontSize);
		FontManager::PushFont("Regular", font0);
		FontManager::PushFont("Bold", font1);
		FontManager::PushFont("Arrows", font2);
		FontManager::SetDefaultFont("Regular");


		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460 core");
	}

	void ImGuiLayer::OnDetach()
	{
		UNEYE_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
	void ImGuiLayer::Begin()
	{
		UNEYE_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		UNEYE_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	uint32_t ImGuiLayer::GetActiveWidgetID() const
	{
		return GImGui->ActiveId;
	}

}
