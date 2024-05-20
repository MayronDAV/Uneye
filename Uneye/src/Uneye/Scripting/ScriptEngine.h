#pragma once

#include <filesystem>
#include <string>

#pragma region MonoStuff

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
}

#pragma endregion

namespace Uneye
{

	class ScriptEngine
	{
		public:
			static void Init();
			static void Shutdown();

			static void LoadAssembly(const std::filesystem::path& filepath);

		private:
			static void InitMono();
			static void ShutdownMono();
			
			static MonoObject* InstantiateClass(MonoClass* monoClass);
	};

	class ScriptClass
	{
		public:
			ScriptClass() = default;

			ScriptClass(const std::string& classNamespace, const std::string& className, bool instantiate = true);

			MonoClass* GetClass();
			MonoObject* GetInstance();

			MonoObject* CallMethod(const std::string& methodName, int param_count = 0, void** params = nullptr, MonoObject** exc = nullptr);

			MonoMethod* GetMethod(const std::string& methodName, int param_count = 0);

			MonoObject* InvokeMethod(MonoMethod* method, void** params = nullptr, MonoObject** exc = nullptr);

		private:
			std::string m_Namespace = std::string();
			std::string m_Name = std::string();
			bool m_Instantiate = true;

			MonoClass* m_MonoClass = nullptr;
			MonoObject* m_ClassObject = nullptr;
	};
}
