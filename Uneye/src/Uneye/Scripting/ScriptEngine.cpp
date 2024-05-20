#include "uypch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"



namespace Uneye
{
	namespace Utils
	{
		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			UNEYE_CORE_ASSERT(!stream, "");

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();

			UNEYE_CORE_ASSERT(size == 0, "");

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath.string(), &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				UNEYE_CORE_CRITICAL(errorMessage);
				return nullptr;
			}

			std::string pathStr = assemblyPath.string();

			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathStr.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}

		static void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				printf("%s.%s\n", nameSpace, name);
			}
		}


	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntitySubClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;

		// Runtime
		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;

	#pragma region ScriptEngine

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();

		LoadAssembly("Resources/Scripts/Uneye-ScriptCore.dll");

		LoadAssemblyClasses(s_Data->CoreAssembly);

		Utils::PrintAssemblyTypes(s_Data->CoreAssembly);

		ScriptGlue::RegisterFunction();

		s_Data->EntityClass = ScriptClass("Uneye", "Entity");

		MonoObject* instance = s_Data->EntityClass.Instantiate();

		//UNEYE_CORE_ASSERT(true, "");
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("UneyeJITRuntime");
		
		UNEYE_CORE_ASSERT(rootDomain == nullptr, "");

		s_Data->RootDomain = rootDomain;
	}

	void ScriptEngine::ShutdownMono()
	{
		// WHY, WHY MONO
		// maybe come back to this

		//mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		//mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}


	void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppDomain = mono_domain_create_appdomain("UneyeScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Move this maybe
		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		//Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;

		s_Data->EntityInstances.clear();
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::GetEntitySubClasses()
	{
		return s_Data->EntitySubClasses;
	}

	void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly)
	{
		s_Data->EntitySubClasses.clear();

		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(image, "Uneye", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, name);
			else
				fullName = name;

			MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (isEntity)
				s_Data->EntitySubClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);
		}
	}

	bool ScriptEngine::EntitySubClassExists(const std::string& fullClassName)
	{
		return s_Data->EntitySubClasses.find(fullClassName) != s_Data->EntitySubClasses.end();
	}

	void ScriptEngine::OnCreateEntity(Entity entt)
	{
		const auto& sc = entt.GetComponent<ScriptComponent>();
		if (ScriptEngine::EntitySubClassExists(sc.Name))
		{
			UUID enttID = entt.GetUUID();
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntitySubClasses[sc.Name], entt);
			s_Data->EntityInstances[enttID] = instance;

			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entt, Timestep ts)
	{
		UNEYE_CORE_ASSERT(s_Data->EntityInstances.find(entt.GetUUID()) == s_Data->EntityInstances.end(), "");

		s_Data->EntityInstances[entt.GetUUID()]->InvokeOnUpdate(ts);
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* classInstance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(classInstance);

		return classInstance;
	}

	#pragma endregion


	#pragma region ScriptClass

	ScriptClass::ScriptClass( const std::string& classNamespace, const std::string& className)
		: m_Namespace(classNamespace), m_Name(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());

	}

	MonoClass* ScriptClass::GetClass() { return m_MonoClass; }

	MonoObject* ScriptClass::Instantiate()
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, m_MonoClass);
		mono_runtime_object_init(instance);

		return instance;
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& methodName, int param_count)
	{
		return mono_class_get_method_from_name(m_MonoClass, methodName.c_str(), param_count);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params, MonoObject** exc)
	{
		return mono_runtime_invoke(method, instance, params, exc);
	}

	#pragma endregion


	#pragma region ScriptInstance

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		:m_ScriptClass(scriptClass)
	{
		m_Instance		 = m_ScriptClass->Instantiate();

		m_Constructor    = s_Data->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = m_ScriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = m_ScriptClass->GetMethod("OnUpdate", 1);

		// Call the entt constructor
		{
			UUID enttID = entity.GetUUID();
			void* param = &enttID;
			m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		void* param = &ts;

		m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
	}

	#pragma endregion
}
