#pragma once

#include "Uneye/Scene/Scene.h"
#include "Uneye/Scene/Entity.h"

#include <filesystem>
#include <string>

#include <unordered_map>
#include <map>

#pragma region MonoStuff

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}

#pragma endregion

namespace Uneye
{
	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;

		MonoClassField* ClassField;
	};

	class ScriptClass
	{
		public:
			ScriptClass() = default;

			ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

			MonoClass* GetClass();

			MonoObject* Instantiate();

			MonoMethod* GetMethod(const std::string& methodName, int param_count = 0);

			MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr, MonoObject** exc = nullptr);

			const std::map<std::string, ScriptField>& GetFields() { return m_Fields; }


		private:
			std::string m_Namespace = std::string();
			std::string m_Name = std::string();

			std::map<std::string, ScriptField> m_Fields;

			MonoClass* m_MonoClass = nullptr;

			friend class ScriptEngine;
	};

	class ScriptInstance
	{
		public:
			ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

			void InvokeOnCreate();
			void InvokeOnUpdate(float ts);

			Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }

			template<typename T>
			T GetFieldValue(const std::string& name)
			{
				bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
				if (!success)
					return T();
				
				return *(T*)s_FieldValueBuffer;
			}

			template<typename T>
			void SetFieldValue(const std::string& name, const T& value)
			{
				SetFieldValueInternal(name, &value);
			}

		private:
			bool GetFieldValueInternal(const std::string& name, void* buffer);
			bool SetFieldValueInternal(const std::string& name, const void* value);

		private:

			Ref<ScriptClass> m_ScriptClass; 

			MonoObject* m_Instance = nullptr;
			MonoMethod* m_Constructor = nullptr;
			MonoMethod* m_OnCreateMethod = nullptr;
			MonoMethod* m_OnUpdateMethod = nullptr;

			inline static char s_FieldValueBuffer[8];
	};

	//class Scene;
	//class Entity;

	class ScriptEngine
	{
		public:
			static void Init();
			static void Shutdown();

			static void LoadAssembly(const std::filesystem::path& filepath);
			static void LoadAppAssembly(const std::filesystem::path& filepath);

			static void OnRuntimeStart(Scene* scene);
			static void OnRuntimeStop();

			static void LoadAssemblyClasses();


			static bool EntitySubClassExists(const std::string& fullClassName);

			static void OnCreateEntity(Entity entt);
			static void OnUpdateEntity(Entity entt, Timestep ts);

			static Scene* GetSceneContext();

			static Ref<ScriptInstance> GetEntityScriptInstance(UUID enttID);

			static std::unordered_map<std::string, Ref<ScriptClass>> GetEntitySubClasses();
			
			static MonoImage* GetCoreAssemblyImage();

		private:
			static void InitMono();
			static void ShutdownMono();

			static MonoObject* InstantiateClass(MonoClass* monoClass);

			friend class ScriptClass;
			friend class ScriptGlue;
	};
}
