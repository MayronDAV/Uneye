#pragma once

#include "Uneye/Scene/Scene.h"
#include "Uneye/Scene/Entity.h"

#include <filesystem>
#include <string>

#include <unordered_map>

#pragma region MonoStuff

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
}

#pragma endregion

namespace Uneye
{


	class ScriptClass
	{
		public:
			ScriptClass() = default;

			ScriptClass(const std::string& classNamespace, const std::string& className);

			MonoClass* GetClass();

			MonoObject* Instantiate();

			MonoMethod* GetMethod(const std::string& methodName, int param_count = 0);

			MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr, MonoObject** exc = nullptr);

		private:
			std::string m_Namespace = std::string();
			std::string m_Name = std::string();

			MonoClass* m_MonoClass = nullptr;
	};

	class ScriptInstance
	{
		public:
			ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

			void InvokeOnCreate();
			void InvokeOnUpdate(float ts);

		private:
			Ref<ScriptClass> m_ScriptClass;

			MonoObject* m_Instance = nullptr;
			MonoMethod* m_Constructor = nullptr;
			MonoMethod* m_OnCreateMethod = nullptr;
			MonoMethod* m_OnUpdateMethod = nullptr;
	};

	//class Scene;
	//class Entity;

	class ScriptEngine
	{
		public:
			static void Init();
			static void Shutdown();

			static void LoadAssembly(const std::filesystem::path& filepath);

			static void OnRuntimeStart(Scene* scene);
			static void OnRuntimeStop();

			static void LoadAssemblyClasses(MonoAssembly* assembly);


			static bool EntitySubClassExists(const std::string& fullClassName);

			static void OnCreateEntity(Entity entt);
			static void OnUpdateEntity(Entity entt, Timestep ts);

			static Scene* GetSceneContext();
			static std::unordered_map<std::string, Ref<ScriptClass>> GetEntitySubClasses();
		private:
			static void InitMono();
			static void ShutdownMono();

			static MonoObject* InstantiateClass(MonoClass* monoClass);

			friend class ScriptClass;
	};
}
